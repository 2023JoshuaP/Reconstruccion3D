# Proyecto de Reconstrucción 3D (OpenGL)

## Descripción del Proyecto
En este repositorio se encuentra implementado un **Sistema de Reconstrucción 3D** a partir de órganos para una Rana. Este proyecto se realizó para el curso de **Computación Gráfica** de la Escuela Profesional de Ciencia de la Computación  de la **Universidad Nacional de San Agustín de Arequipa** por los estudiantes:

- ***Josue Samuel Philco Puma*:** (https://github.com/2023JoshuaP)
- ***Aldo Raúl Martínez Choque*:** (https://github.com/ALdoMartineCh16)

### Propósito del Proyecto

Este repositorio es realizado como una implementación para poder **reconstruir un modelo 3D** a partir de archivos **.tiff** multipágina con máscaras de órganos de una Rana. Este está diseñado como una guía y ayudar a comprender la lógica para una reconstrucción 3D.

**Nota Importante:** Se alienta a los usuarios a usar este repositorio como referencia para inspirarse y aprender, pero se desaconseja su uso para copiar y presentar el trabajo como propio. El propósito principal es servir de ayuda y guía en el proceso de aprendizaje.

## Características Principales

1. **Extracción de Puntos**: A partir de las máscaras de los órganos en los archivos proporcionados se empieza a extraer todos los puntos blancos, para ello usamos la librería **libtiff** que nos va a permitir leer los archivos (otra opción es usar OpenCV para extraer los puntos o convertirlo a imágenes cada archivo) para luego almacenarlos en un archivo **.xyz** que contiene las coordenadas **X** y **Y**, y para la coordenada **Z** es el índice de página donde están los puntos.

2. **Procesamiento de los Puntos**: Usamos el algoritmo de **Marching Cubes** para realizar un procesamiento de los archivos **.xyz** para generar una malla 3D con volumen que demuestre el modelo 3D, como son miles de puntos a procesar optamos por estructuras de optimización como **Hash**, **Función Gaussiana** e **Interpolación Lineal** para obtener un modelo bien hecho sin huecos o con caras cuadrangulares. Al momento de procesar los puntos los vamos almacenando en un archivo **.obj** con todo el procesamiento realizado.

3. **Reconstrucción con OpenGL**: Esta es la parte más importante para nuestro modelo, ya que con esto mostraremos todos nuestros **.obj** en la pantalla con la reconstrucción. Aclaro que tenemos un total de 17 órganos y para ir reconstruyendo nuestra Rana hicimos una lista con todos los órganos junto con atributos de posición, color, escala, clave de teclado para mostrar y una descripción. También podemos visualizar la malla generada con el procesamiento, mostrar todos los órganos, mostrar un órgano, realizar zoom al modelo y realizar rotaciones. También para guiar al usuario está presente una guía de las opciones que se pueden hacer en la consola.

## Manual de usuario y ejecución

Este proyecto fue desarrollado en Windows, por lo que se detalla lo necesario para poder ejecutar el proyecto.

1. **Compilador**

El compilador usado fue **MSYS2 MinGW64** para C++ como lenguaje principal de este proyecto.

2. **Librerías requeridas**

Como se mencionó, se requiere de librerías como **libtiff** y también para OpenGL como **GLAD, GLM y GLFW3**, cada una de ellas puede ser instalada desde el **shell** del compilador o a partir de los sitios oficiales. Por ejemplo, **libtiff** puede ser instalado como:

``
pacman -S mingw-w64-x86_64-libtiff
``

3. **Ejecución**

Para ejecutar la parte de **Extracción de Puntos** y **Procesamiento de Puntos** lo hacemos desde el archivo ***main.cpp***. Los comandos para compilar los dos procesos y visualización son los siguiente:

- **Extracción y Procesamiento de Puntos**

``
g++ src/main.cpp src/Extraction_Points_Tiff.cpp src/Marching_Cubes.cpp -Iheaders -Iinclude -Llib -ltiff -o main.exe
``

- **Visualizador OpenGL**

``
g++ src/Visualizer_update.cpp src/glad.c -Iinclude -Llib -lglfw3dll -lglew32 -lopengl32 -lgdi32 -ldwmapi -ltiff -o Visualizer_update.exe
``

Para poder ejecutar los dos procesos se hace el siguiente comando:

- **Extracción de Puntos**

``
./main.exe public/[organo].tiff 1
``
    
- **Procesamiento de Puntos**

``
./main.exe coordenadas/[organo]Masks_extraction_points.xyz 2
``

- **Visualización de Órganos**

``
./Visualizer_update.exe
``

Al momento en que se ejecuta puede tardar un tiempo en procesar los puntos o archivos. Todo el proceso lo realiza mediante CPU y memoria RAM.