# Antifurto
This project is a security camera implemented in C++. It is optimized for
Raspberry PI, but it can potentially run on any Linux machine with an USB
camera. Porting it to Windows would be a bit more involved.

The antifurto project is essentially a security camera that allows to monitor
what happens through the lenses of a single camera. When the camera detects
motion above a certain threshold, it sends notifications through WhatsApp and
emails and starts to record pictures. These are in turn saved to the local disk
and uploaded to a Dropbox folder. There is also a web portal (optimized for
desktop and mobile browsers) from which you can start and stop the monitoring, a
live view from which you can see images in real time and an archive page for the
past recordings. It's not possible to combine multiple cameras together: single
camera, single website.

For more details on the architecture see the blog post
[Antifurto: home made security camera](http://blog.mbrt.it/2017-07-07-antifurto-code-review/).

# Build
Install dependencies (on Debian or Ubuntu):
```
> sudo apt-get install boost-dev libzmq3-dev libfcgi-dev opencv-dev cmake g++
```

Build:
```
> mkdir build
> cd build
> cmake ..
> make
```

The build for Raspberry PI requires cross compilation on your laptop or native
compilation (slow) directly in the Raspberry PI. For the first options I
provided a toolchain file as an example in `toolchain-pi.cmake`. You'll need to
customize it a bit for your environment.

## Webserver install
See https://wiki.ubuntu.com/Lighttpd+PHP
```
> sudo apt-get install lighttpd php5-cgi
> sudo lighty-enable-mod fastcgi
> sudo lighty-enable-mod fastcgi-php
> sudo lighty-enable-mod auth
> sudo service lighttpd force-reload
```

Copy the web interface in `/var/www`:
```
> cp -r webpages/* /var/www/
```

Copy the fcgi frontend:
```
> sudo mkdir /var/www/webapps/
> sudo cp /build/dir/frontend/antifurto_fcgi /var/www/webapps/
```

Link the archive dir:
```
> ln -s /path/to/archive /var/www/archive/pics
```

Apply configuration:
```
> cd /etc/lighttpd/
> sudo cp lighttpd.conf lighttpd.conf.bak
```

Use one of the following options:
option 1: patch
```
    > diff -Naur lighttpd.conf /path/to/project/config/lighttpd.conf.patched > patch
    > sudo patch lighttpd < patch
```
option 2: merge
```
    > sudo merge lighttpd.conf /.../lighttpd.conf.original /.../lighttpd.conf.patched
```
option 3: overwrite
```
    > sudo cp /.../lighttpd.conf.patched lighttpd.conf
```

Patch in the same way the file `/etc/lighttpd/conf-enabled/05-auth.conf`:

Password file:
```
> sudo apt-get install apache2-utils
> htdigest -c lighttpd.user.htdigest 'admin area' admin
  type a password for the user
> sudo mv lighttpd.user.htdigest /etc/lighttpd/
> sudo chown root:root /etc/lighttpd/lighttpd.user.htdigest
```

Restart the service:
```
> sudo service lighttpd restart
```

Add permissions to www-data to send signals to antifurto:
```
> sudo visudo -f /etc/sudoers.d/antifurto
  add the following line to the file
    www-data myhost = (root) NOPASSWD: /usr/bin/killall
  where myhost is the hostname (e.g. rasberry)
```

## Send mail install
See: https://rtcamp.com/tutorials/linux/ubuntu-postfix-gmail-smtp/

Install packages:
```
> sudo apt-get install postfix mailutils libsasl2-2 ca-certificates libsasl2-modules
```

In configuration, select "Internet Site" and an hostname.
Then configure postfix to use your mail address to send mails.
You can use for example gmail.

Add these lines to `/etc/postfix/main.cf`:

```
relayhost = [smtp.gmail.com]:587
smtp_sasl_auth_enable = yes
smtp_sasl_password_maps = hash:/etc/postfix/sasl_passwd
smtp_sasl_security_options = noanonymous
smtp_tls_CAfile = /etc/postfix/cacert.pem
smtp_use_tls = yes
```

Then set your usename and password in this way:
```
> echo "[smtp.gmail.com]:587    USERNAME@gmail.com:PASSWORD" > /etc/postfix/sasl_passwd
```

Fix permissions:
```
> sudo chmod 400 /etc/postfix/sasl_passwd
```

Validate certificates:
```
> cat /etc/ssl/certs/Thawte_Premium_Server_CA.pem | sudo tee -a /etc/postfix/cacert.pem
```

Reload postfix:
```
> sudo postmap /etc/postfix/sasl_passwd
> sudo /etc/init.d/postfix reload
```

## Antifurto install

Setup init script:
```
- edit /etc/rc.local
- add this line before "exit 0"
  /path/to/antifurto &
> sudo cp /path/to/project/script/init.sh /etc/init.d/antifurto
> cd /etc/init.d/
> sudo chmod 755 antifurto
> sudo update-rc.d antifurto defaults
```

Install zeromq dependencies:
```
> sudo apt-get install libzmq3
```

Install fcgi lib:
```
> sudo apt-get install libfcgi
```

Logrotate script
```
> cp /path/to/project/config/antifurto_logrotate /etc/logrotate.d/antifurto
```

Setup core file limit:
- edit `/etc/rc.local`
- add this line before the "antifurto" startup command:
```
  ulimit -c unlimited
```
- then create:
```
> sudo mkdir /var/log/dumps
```
- edit `/etc/sysctl.conf`
- add this line at the end (if not already present)
```
  kernel.core_pattern=/var/log/dumps/core.%e.%p
```

Cron script:
- copy crontab script
```
> sudo cp /path/to/project/config/antifurto_cron /etc/cron.d/antifurto
```
