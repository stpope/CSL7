/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

class SharedValueSourceUpdater  : public ReferenceCountedObject,
                                  private AsyncUpdater
{
public:
    SharedValueSourceUpdater() : sourcesBeingIterated (nullptr) {}
    ~SharedValueSourceUpdater()  { masterReference.clear(); }

    void update (Value::ValueSource* const source)
    {
        sourcesNeedingAnUpdate.add (source);

        if (sourcesBeingIterated == nullptr)
            triggerAsyncUpdate();
    }

    void valueDeleted (Value::ValueSource* const source)
    {
        sourcesNeedingAnUpdate.removeValue (source);

        if (sourcesBeingIterated != nullptr)
            sourcesBeingIterated->removeValue (source);
    }

    WeakReference<SharedValueSourceUpdater>::Master masterReference;

private:
    typedef SortedSet<Value::ValueSource*> SourceSet;
    SourceSet sourcesNeedingAnUpdate;
    SourceSet* sourcesBeingIterated;

    void handleAsyncUpdate()
    {
        const ReferenceCountedObjectPtr<SharedValueSourceUpdater> localRef (this);

        {
            const ScopedValueSetter<SourceSet*> inside (sourcesBeingIterated, nullptr, nullptr);
            int maxLoops = 10;

            while (sourcesNeedingAnUpdate.size() > 0)
            {
                if (--maxLoops == 0)
                {
                    triggerAsyncUpdate();
                    break;
                }

                SourceSet sources;
                sources.swapWith (sourcesNeedingAnUpdate);
                sourcesBeingIterated = &sources;

                for (int i = sources.size(); --i >= 0;)
                    if (i < sources.size())
                        sources.getUnchecked(i)->sendChangeMessage (true);
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SharedValueSourceUpdater);
};

static WeakReference<SharedValueSourceUpdater> sharedUpdater;

Value::ValueSource::ValueSource()
{
}

Value::ValueSource::~ValueSource()
{
    if (asyncUpdater != nullptr)
        static_cast <SharedValueSourceUpdater*> (asyncUpdater.get())->valueDeleted (this);
}

void Value::ValueSource::sendChangeMessage (const bool synchronous)
{
    const int numListeners = valuesWithListeners.size();

    if (numListeners > 0)
    {
        if (synchronous)
        {
            const ReferenceCountedObjectPtr<ValueSource> localRef (this);
            asyncUpdater = nullptr;

            for (int i = numListeners; --i >= 0;)
                if (Value* const v = valuesWithListeners[i])
                    v->callListeners();
        }
        else
        {
            SharedValueSourceUpdater* updater = static_cast <SharedValueSourceUpdater*> (asyncUpdater.get());

            if (updater == nullptr)
            {
                if (sharedUpdater == nullptr)
                {
                    asyncUpdater = updater = new SharedValueSourceUpdater();
                    sharedUpdater = updater;
                }
                else
                {
                    asyncUpdater = updater = sharedUpdater.get();
                }
            }

            updater->update (this);
        }
    }
}

//==============================================================================
class SimpleValueSource  : public Value::ValueSource
{
public:
    SimpleValueSource()
    {
    }

    SimpleValueSource (const var& initialValue)
        : value (initialValue)
    {
    }

    var getValue() const
    {
        return value;
    }

    void setValue (const var& newValue)
    {
        if (! newValue.equalsWithSameType (value))
        {
            value = newValue;
            sendChangeMessage (false);
        }
    }

private:
    var value;

    JUCE_DECLARE_NON_COPYABLE (SimpleValueSource);
};


//==============================================================================
Value::Value()
    : value (new SimpleValueSource())
{
}

Value::Value (ValueSource* const value_)
    : value (value_)
{
    jassert (value_ != nullptr);
}

Value::Value (const var& initialValue)
    : value (new SimpleValueSource (initialValue))
{
}

Value::Value (const Value& other)
    : value (other.value)
{
}

Value& Value::operator= (const Value& other)
{
    value = other.value;
    return *this;
}

#if JUCE_COMPILER_SUPPORTS_MOVE_SEMANTICS
Value::Value (Value&& other) noexcept
    : value (static_cast <ReferenceCountedObjectPtr <ValueSource>&&> (other.value))
{
}

Value& Value::operator= (Value&& other) noexcept
{
    value = static_cast <ReferenceCountedObjectPtr <ValueSource>&&> (other.value);
    return *this;
}
#endif

Value::~Value()
{
    if (listeners.size() > 0)
        value->valuesWithListeners.removeValue (this);
}

//==============================================================================
var Value::getValue() const
{
    return value->getValue();
}

Value::operator var() const
{
    return value->getValue();
}

void Value::setValue (const var& newValue)
{
    value->setValue (newValue);
}

String Value::toString() const
{
    return value->getValue().toString();
}

Value& Value::operator= (const var& newValue)
{
    value->setValue (newValue);
    return *this;
}

void Value::referTo (const Value& valueToReferTo)
{
    if (valueToReferTo.value != value)
    {
        if (listeners.size() > 0)
        {
            value->valuesWithListeners.removeValue (this);
            valueToReferTo.value->valuesWithListeners.add (this);
        }

        value = valueToReferTo.value;
        callListeners();
    }
}

bool Value::refersToSameSourceAs (const Value& other) const
{
    return value == other.value;
}

bool Value::operator== (const Value& other) const
{
    return value == other.value || value->getValue() == other.getValue();
}

bool Value::operator!= (const Value& other) const
{
    return value != other.value && value->getValue() != other.getValue();
}

//==============================================================================
void Value::addListener (ValueListener* const listener)
{
    if (listener != nullptr)
    {
        if (listeners.size() == 0)
            value->valuesWithListeners.add (this);

        listeners.add (listener);
    }
}

void Value::removeListener (ValueListener* const listener)
{
    listeners.remove (listener);

    if (listeners.size() == 0)
        value->valuesWithListeners.removeValue (this);
}

void Value::callListeners()
{
    if (listeners.size() > 0)
    {
        Value v (*this); // (create a copy in case this gets deleted by a callback)
        listeners.call (&ValueListener::valueChanged, v);
    }
}

OutputStream& JUCE_CALLTYPE operator<< (OutputStream& stream, const Value& value)
{
    return stream << value.toString();
}
