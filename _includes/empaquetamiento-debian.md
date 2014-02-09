
### Herramientas para descargar código fuente del empaquetador debian para libre-impuestos

Los siguientes métodos son aplicables:

1) [git](http://git-scm.com/) es software libre y código abierto, es un sistema distribuido de control de versiones diseñado para manejar todo, desde pequeños a grandes proyectos con velocidad y eficiencia.

Instalar la herramienta:
```
$ sudo apt-get install git
```

Descargar el codigo fuente usando git:

```
$ git clone https://github.com/hcvcastro/libre-impuestos-debian.git
```
2) [cURL](http://curl.haxx.se/) es una herramienta para usar en un intérprete de comandos para transferir archivos con sintaxis URL, soporta FTP, FTPS, HTTP, HTTPS, TFTP, SCP, SFTP, Telnet, DICT, FILE y LDAP. [cURL](http://curl.haxx.se/) es open source/software libre distribuido bajo la Licencia MIT.


Instalar la herramienta:
```
$ sudo apt-get install curl
```
Descargar el código fuente en formato tar.gz usando wget:

```
$ curl -L -o libre-impuesto-debian-source.tar.gz https://github.com/hcvcastro/libre-impuestos-debian/tarball/master
```

Descargar el código fuente en formato zip usando wget:

```
$ curl -L -o libre-impuesto-debian-source.zip https://github.com/hcvcastro/libre-impuestos-debian/zipball/master
```

Después de descargar debe descompimir los archivos en su directorio de preferencia.

3) Usar el Navegador de Internet de preferencia (browser), ir a <http://github.com/hcvcastro/libre-impuestos-debian> click el boton "Download ZIP".


### Empaquetamiento Debian de libre-impuestos

Para empaquetar el proyecto libre-impuestos tenemos que tener instalados las herramientas de empaquetamiento [build-essential](http://packages.debian.org/build-essential), [devscripts](http://packages.debian.org/devscripts) y [debhelper] (http://packages.debian.org/debhelper)

Suponiendo que el código fuente esta ubicado en el directorio:

```
~$ cd libre-impuestos-debian
~/libre-impuestos-debian$ ls -d */
```
Verificamos la salida:

```
debian/
```

Ejecutamos unpack del makefile rules:

```
~/libre-impuestos-debian$ debian/rules unpack
```

Despues de terminar con éxito:

```
~/libre-impuestos-debian$ dpkg-buildpackage -uc -us
```

Luego de terminar con éxito el empaquetamiento verificamos los archivos deb:

```
~/libre-impuestos-debian$ ls ../*.deb
```

### Instalación del paquete libre-impuestos


```
~/libre-impuestos-debian$ cd ..
~$ sudo dpkg -i libre-impuestos_0.0.3-1_amd64.deb
```

### Ejecutar libre-impuestos


Después de terminar con éxito la instalación, ejecutamos la aplicación:

```
$ libre-impuestos
```
