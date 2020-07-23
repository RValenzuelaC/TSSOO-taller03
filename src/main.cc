#include <global.hh>
#include <checkArgs.hpp>
uint64_t sumasSecuenciales = 0;
uint64_t *sumasParalelas = nullptr;
uint64_t *sumasOpenMp = nullptr;
uint64_t *arreglo_p = nullptr;
uint64_t *arreglo_s = nullptr;
void sumaSecuencial(size_t pos,
				size_t beginIndex,
				size_t endIndex)
{
	for (size_t i = beginIndex; i < endIndex; ++i)
	{
		sumasSecuenciales += arreglo_p[i];
	}
}
void sumaParalela(size_t pos,
				  size_t beginIndex,
				  size_t endIndex)
{
	for (size_t i = beginIndex; i < endIndex; ++i)
	{
		sumasParalelas[pos] += arreglo_p[i];
	}
}

void llenadoAr(size_t beginIdx, size_t endIdx, uint32_t limiteInferior, uint32_t limiteSuperior, size_t Rnetype)
{
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<uint32_t> nRandom(limiteInferior, limiteSuperior);
	for (size_t i = beginIdx; i < endIdx; ++i)
	{
		uint32_t random = nRandom(rng);
		switch (Rnetype)
		{
		case 0:
			arreglo_p[i] = random;

			break;

		case 1:
			arreglo_s[i] = random;
			break;
		}
	}
}

int main(int argc, char **argv)
{

	uint64_t totalElementos;
	uint64_t numThreads;
	uint32_t limiteInferior;
	uint32_t limiteSuperior;
	uint64_t sumaThreads = 0;
	std::vector<std::thread *> threads;
	std::vector<std::thread *> threads2;

	auto argumentos = (std::shared_ptr<checkArgs>)new checkArgs(argc, argv);

	totalElementos = argumentos->getArgs().tamProblema;
	numThreads = argumentos->getArgs().numThreads;
	limiteInferior = argumentos->getArgs().limiteInferior;
	limiteSuperior = argumentos->getArgs().limiteSuperior;
	//--------------------------------------------------------llenado del arreglo en secuencia--------------------------------------------------------
	arreglo_s = new uint64_t[totalElementos];
	auto start = std::chrono::high_resolution_clock::now();
	llenadoAr(0, totalElementos, limiteInferior, limiteSuperior, 1);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> elapsed= end - start;
	auto totalTimeSerial = elapsed.count();

	//--------------------------------------------------------llenado del arreglo paralelo---------------------------------------------------------

	arreglo_p = new uint64_t[totalElementos];
	for (size_t i = 0; i < numThreads; ++i)
	{
		threads.push_back(new std::thread(llenadoAr,
										  i * (totalElementos) / numThreads,
										  (i + 1) * (totalElementos) / numThreads, limiteInferior, limiteSuperior, 0));
	}
	//-------------------------------------------------tiempo de llenado paralelo-------------------------------------------------------------------
	start = std::chrono::system_clock::now();
	for (auto &thrs : threads)
	{
		thrs->join();
	}
	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duration = end - start;
	auto totalTimefilled = duration.count();

	//---------------------------------------------------------llenado openMP----------------------------------------------------------------------
	
	auto vector_l = new uint64_t[totalElementos];
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<uint32_t> nRandom(limiteInferior, limiteSuperior);
	start = std::chrono::system_clock::now();
	#pragma omp parallel for  num_threads(numThreads)
	for(size_t i = 0; i < totalElementos; ++i){	
		vector_l[i] = nRandom(rng);
	}
	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duration2 = end - start;
	auto totalTimeOpmp = duration2.count();
	//------------------------------------------suma Threads---------------------------------------------------------------------------------------
	
	sumasParalelas = new uint64_t[totalElementos];
	for (size_t i = 0; i < numThreads; ++i)
	{

		threads2.push_back(new std::thread(sumaParalela,
										   i,
										   i * (totalElementos) / numThreads,
										   (i + 1) * (totalElementos) / numThreads));
	}
	//-----------------------------------------------------Tiempo de suma Threads-----------------------------------------------------------------
	start = std::chrono::system_clock::now();
	for (auto &thr : threads2)
	{
		thr->join();
	}
	for (size_t i = 0; i < numThreads; i++)
	{
		sumaThreads += sumasParalelas[i];
	}
	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duration3 = end - start;
	auto totalTimeThread = duration3.count();
	//--------------------------------------------Tiempo de sumaSecuencial-----------------------------------------------------------------
	start = std::chrono::high_resolution_clock::now();
	sumaSecuencial(0,0,totalElementos);
	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duration4 = end - start;
	auto totalTimeSumSerial = duration4.count();
	//---------------------------------------------tiempo suma openmp----------------------------------------------------------------------
	uint64_t count= 0;
	start = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for reduction(+:count) num_threads(numThreads)
	for(size_t i = 0; i < totalElementos; ++i){
		count += vector_l[i];
	}
	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duration5 = end - start;
	auto totalTimeOmp = duration5.count();

	

	std::cout << "Elementos: " << totalElementos << std::endl;
	std::cout << "Threads  : " << numThreads << std::endl;
	std::cout << "limite inferior: " << limiteInferior << std::endl;
	std::cout << "limite Superior: " << limiteSuperior << std::endl;

	std::cout << "==========Suma Total=================" << std::endl;
	std::cout << "Suma en Secuencial: " << sumasSecuenciales << std::endl;
	std::cout << "Suma en Paralelo: " << sumaThreads << std::endl;
	std::cout << "Suma en Omp: " << count << std::endl;

	std::cout << "==========Tiempos de Llenado=========" << std::endl;
	std::cout << "TiempodeLLenadoSecuencial: " << totalTimeSerial << "[ms]" << std::endl;
	std::cout << "TiempodeLLenadoParalelo: " << totalTimefilled << "[ms]" << std::endl;
	std::cout << "TiempodeLLenadoOpenMp: " << totalTimeOpmp  << "[ms]" << std::endl;
    std::cout << "SpeedUp Etapa de Llenado Thr vs Serial: " << (double)totalTimeSerial/totalTimefilled << std::endl;
	std::cout << "SpeedUp Etapa de Llenado Omp vs Serial : " << (double)totalTimeSerial/totalTimeOpmp << std::endl;
	std::cout << "==========Tiempos de Sumado==========" << std::endl;
	std::cout << "TiempoDeSumadoSecuencial: " << totalTimeSumSerial << "[ms]" << std::endl;
	std::cout << "TiempoDeSumadoParalelo: " << totalTimeThread << "[ms]" << std::endl;
	std::cout << "TiempoDeSumadoOpenMp: " << totalTimeOmp << "[ms]" << std::endl;
    std::cout << "SpeedUp Etapa de Suma Thr vs Serial: " << (double)totalTimeSumSerial/totalTimeThread << std::endl;
	std::cout << "SpeedUp Etapa de Suma Omp vs Serial: " << (double)totalTimeSumSerial/totalTimeOmp << std::endl;
	return (EXIT_SUCCESS);
}