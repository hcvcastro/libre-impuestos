
### Herramientas para descargar código fuente libre-impuestos

Los siguientes métodos son aplicables:

1) [git](http://git-scm.com/) es software libre y código abierto, es un sistema distribuido de control de versiones diseñado para manejar todo, desde pequeños a grandes proyectos con velocidad y eficiencia.

Instalar la herramienta:
```
$ sudo apt-get install git
```

Descargar el codigo fuente usando git:

```
$ git clone https://github.com/hcvcastro/libre-impuestos.git
```


2) [cURL](http://curl.haxx.se/) es una herramienta para usar en un intérprete de comandos para transferir archivos con sintaxis URL, soporta FTP, FTPS, HTTP, HTTPS, TFTP, SCP, SFTP, Telnet, DICT, FILE y LDAP. [cURL](http://curl.haxx.se/) es open source/software libre distribuido bajo la Licencia MIT.


Instalar la herramienta:
```
$ sudo apt-get install curl
```
Descargar el código fuente en formato tar.gz usando wget:

```
$ curl -L -o libre-impuesto-source.tar.gz https://github.com/hcvcastro/libre-impuestos/tarball/master
```

Descargar el código fuente en formato zip usando wget:

```
$ curl -L -o libre-impuesto-source.zip https://github.com/hcvcastro/libre-impuestos/zipball/master
```

Después de descargar debe descompimir los archivos en su directorio de preferencia.

3) Usar el Navegador de Internet de preferencia (browser), ir a <http://github.com/hcvcastro/libre-impuestos> click el boton "Download ZIP".


### Compilación de libre-impuestos

Para compilar el proyecto libre-impuestos tenemos que tener instalados las herramientas de compilación, gcc, make, autoconf, etc.

Suponiendo que el código fuente esta ubicado en el directorio:

```
~$ cd libre-impuestos
~/libre-impuestos$ ls
```

Es recomendable que la compilación sea ubicado en otro directorio:

```
~/libre-impuestos$ cd ..
$ mkdir -p build
```

Ahora verificamos nuestra estructura de directorios:

```
$ ls -d */
```

Observará que tenemos dos directorios:

```
libre-impuestos/    
build/		    
```

Ingresamos al directorio build, y ejectutamos autogen.sh:

```
$ cd  build
~/build$ ../libre-impuestos/autogen.sh
```

Finalmente ejecutamos make:

```
~/build$ make
```

En el proceso de compilación, verificara si existen todas las herramientas de compilación,  si falta alguno o tienes problemas, por favor contáctame a Henry Castro <hcvcastro@gmail.com>

### Instalación de libre-impuestos


Después de terminar con éxito la compilación, procedemos a instalar libre-impuestos:

```
~/build$ sudo make install
```

### Ejecutar libre-impuestos


Después de terminar con éxito la instalación, ejecutamos la aplicación:

```
$ libre-impuestos
```

