
%define name    libsamplerate
%define version 0.2.1
%define release 1
%define prefix  /usr

Summary: A library to do sample rate conversion for audio.
Name: %{name}
Version: %{version}
Release: %{release}
Prefix: %{prefix}
Copyright: BSD
Group: Libraries/Sound
Source: http://www.mega-nerd.com/SRC/libsamplerate-%{version}.tar.gz
URL: https://github.com/libsndfile/libsamplerate/
BuildRoot: /var/tmp/%{name}-%{version}

%description
libsamplerate is a C library capable of arbitrary and time varying
conversions; from downsampling by a factor of 12 to upsampling by the
same factor. The conversion ratio can also vary with time for speeding
up and slowing down effects.

%package devel
Summary: Libraries, includes, etc to develop libsamplerate applications
Group: Libraries

%description devel
Libraries, include files, etc you can use to develop libsamplerate applications.

%prep
%setup

%build
./configure --prefix=%{prefix}
make

%install
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi
mkdir -p $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README doc
%prefix/lib/libsamplerate.so.*

%files devel
%defattr(-,root,root)
%{prefix}/lib/libsamplerate.a
%{prefix}/lib/libsamplerate.la
%{prefix}/lib/libsamplerate.so
%{prefix}/include/samplerate.h
%{prefix}/lib/pkgconfig/samplerate.pc

