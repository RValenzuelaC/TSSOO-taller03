# TSSOO-taller03
##### Autor: Raúl  Ignacio Valenzuela Cinto - Raul.valenzuela@alumnos.uv.cl
### 1. Diseño del problema
El diseño de este taller se basa en la creación de un código en OpenMp el cual consistirá de 2 módulos,primeramente se tendrá que verificar si los parámetros ingresados son realmente funcionales, luego estos módulos tendrán que ser, uno de llenado de un arreglo en paralelo como se muestra en la siguiente figura.
 ![modulo1](http://imgfz.com/i/0L8PgWO.png)
 
 Luego de esto se llevará a cabo la suma de este arreglo paralelamente como se muestra en la siguiente figura.
![modulo2](http://imgfz.com/i/PdfDlY7.png)

Terminando así con los módulos solicitados y continuando luego con pruebas de desempeño para analizar los datos entregados.

###   2. Estructura del código 
#### 2.1 Parte de verificación de datos  
Primero que nada se verificarán los datos ingresados como parámetros, si estos cumplen la forma de uso se llevará a cabo el código realizado dando así parámetros para que este haga los módulos antes mencionados, en cambio si los parámetros ingresados estaban erróneos o simplemente no existían, este mandaría la forma de uso como mensaje para entender la forma correcta de utilización. 
Para esta parte, se tuvo que editar el archivo `checkArgs` para poder realizar la confirmación de parametros y la validación de estos. 
#### 2.2 llenado de el arreglo
Para el llenado del arreglo se uso la funcion `#pragma omp parallel` para el llenado de forma paralela, la cual permite paralelizar el codigo que se encuentre dentro de esta, además de ser llenado randomicamente, por los limites definidos por `limiteInferior` y `limiteSuperior`, como se muestra en el siguiente codigo:
```
auto vector_l = new uint64_t[totalElementos];
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<uint32_t> nRandom(limiteInferior, limiteSuperior);
	#pragma omp parallel for  num_threads(numThreads)
	for(size_t i = 0; i < totalElementos; ++i){	
		vector_l[i] = nRandom(rng);
	}
```
#### 2.3 sumado del arreglo 
Para la suma paralela se tomaron los datos almacenados en la variable `arreglo_p` y se guardan en una nueva variable `count` la cual contendrá la suma total implementada con OpenMP como se muestra en el siguiente codigo:
```
uint64_t count= 0;
#pragma omp parallel for reduction(+:count) num_threads(numThreads)
	for(size_t i = 0; i < totalElementos; ++i){
		count += arreglo_p[i];
	}
```
