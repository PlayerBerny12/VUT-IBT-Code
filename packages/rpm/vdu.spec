Name:           vdu
Version:        1.0
Release:        1%{?dist}
Summary:        VDU-APP

License:        GPLv2
Source0:        vdu-1.0.tar.gz

Requires:       fuse3-libs                

%description
VDU-APP for synchronizing data from VDU.

%prep
%setup -q

%install
mkdir -p %{buildroot}/usr/local/bin
mkdir -p %{buildroot}/usr/share/applications/
mkdir -p %{buildroot}/etc/systemd/user/

install -m 4755 vdu-app-fuse %{buildroot}/usr/local/bin/vdu-app-fuse
install -m 755 vdu-app %{buildroot}/usr/local/bin/vdu-app
install -m 644 vdu.desktop %{buildroot}/usr/share/applications/vdu.desktop
install -m 644 vdu.service %{buildroot}/etc/systemd/user/vdu.service

%files
%license LICENSE
/usr/local/bin/vdu-app-fuse
/usr/local/bin/vdu-app
/usr/share/applications/vdu.desktop
/etc/systemd/user/vdu.service

%post
mkdir /mnt/vdu

xdg-mime default vdu.desktop x-scheme-handler/vdu
update-desktop-database
systemctl daemon-reload
systemctl --global enable vdu.service


%changelog
* Mon May 10 2021 Jan Bernard <xberna18@stud.fit.vutbr.cz> 1.0-1
First Release
