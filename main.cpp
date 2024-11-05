/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: 20180954 Lian Abril Tume Cusi
 * Created on 24 de octubre de 2024, 09:11 PM
 */

#include <iostream>
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <stack>
#include <set>

#include "ClPieza.h"
#include "ClBase.h"

#include "ClCromosoma.h"

using namespace std;

int numPiezas = 30;
vector<vector<int>> matriz(numPiezas, vector<int>(numPiezas, -1));
vector<vector<int>> solucion(numPiezas, vector<int>(numPiezas, -1));
int anchoMayor,altoMayor;
double desTotal;

void resetearMatriz(vector<vector<int>>& matriz) {
    for (auto& fila : matriz) {
        for (auto& elemento : fila) {
            elemento = -1;
        }
    }
}

vector<int> convertirMatrizACromosoma(const vector<vector<int>>& matriz) {
    vector<int> lista;
    int filas = matriz.size();
    int columnas = matriz[0].size();

    for (int j = 0; j < columnas; ++j) {       // Recorre las columnas primero
        for (int i = 0; i < filas; ++i) {      // Luego las filas de cada columna
            if (matriz[i][j] != -1) {          // Ignora valores -1
                lista.push_back(matriz[i][j]);
            }
        }
    }
    
    int cantidad_actual = lista.size();
    while (cantidad_actual < numPiezas) {
        lista.push_back(-1);
        cantidad_actual++;
    }

    return lista;
}

void imprimirMatriz(vector<vector<int>>& matriz) {
    for (int j = 0; j < (numPiezas-1); ++j) {
        for (int i = 0; i < (numPiezas-1); ++i) {
            if(matriz[i][j] == -1) break;
            cout << matriz[i][j] << " "; // Debe imprimir mejor matriz, no matriz
            //matriz[i][j] = -1;
        }
        cout << endl;  // Salto de línea después de cada fila
    }
}

vector<Pieza> generarListaPiezas(int cantidad) {
    vector<Pieza> listaPiezas;
    for (int i = 0; i < cantidad; ++i) {
        float x = 0;
        float y = 0;
        float w = (rand() % 50) + 30;
        float h = (rand() % 20) + 10;
        bool rotada = 0;
        
        Pieza p(i, x, y, w, h, rotada);
        listaPiezas.push_back(p);
    }
    return listaPiezas;
}

vector<Stock> generarListaStocks(int cantidad) {
    vector<Stock> listaStocks;
    for (int i = 0; i < cantidad; ++i) {
        int w = (rand() % 200) + 150;
        int h = (rand() % 100) + 50;
        Stock s(w, h);
        listaStocks.push_back(s);
    }
    return listaStocks;
}

bool compararPiezas(const Pieza& a, const Pieza& b) {
    return (a.getH() >b.getH());
}

void ordenarPiezas(vector<Pieza>& listaPiezas) {
    sort(listaPiezas.begin(), listaPiezas.end(), compararPiezas);
}

bool compararStocks(const Stock& a, const Stock& b) {
    return (a.getAncho() * a.getAlto()) > (b.getAncho() * b.getAlto());
}

void ordenarStocks(vector<Stock>& stocks) {
    sort(stocks.begin(), stocks.end(), compararStocks);
}

// Función de evaluación del fitness: Minimizar el desperdicio.
double calcularFitness(const vector<int>& cromosoma, const vector<Stock>& stocks) {
    int desperdicio = 0;
    for (int piezaID : cromosoma) {
        desperdicio += piezaID;
    }
    return 1.0 / (1 + desperdicio);
}


// Cruce por un punto: Intercambiamos segmentos de dos padres.
pair<vector<int>, vector<int>> cruzar(const vector<int>& padre1, const vector<int>& padre2) {
    int puntoCruce = rand() % padre1.size();
    vector<int> hijo1(padre1.begin(), padre1.begin() + puntoCruce);
    vector<int> hijo2(padre2.begin(), padre2.begin() + puntoCruce);

    hijo1.insert(hijo1.end(), padre2.begin() + puntoCruce, padre2.end());
    hijo2.insert(hijo2.end(), padre1.begin() + puntoCruce, padre1.end());

    return make_pair(hijo1, hijo2);
}

// Mutación: Alteramos un gen del cromosoma.
void mutar(vector<int>& cromosoma, int tasaMutacion) {
    for (int& gene : cromosoma) {
        if (rand() / static_cast<double>(RAND_MAX) < tasaMutacion) {
            gene = rand() % numPiezas;
        }
    }
}

// Selección por torneo: Elegimos el mejor entre varios individuos aleatorios.
vector<int> seleccionar(const vector<vector<int>>& poblacion, const vector<double>& fitness) {
    int torneo = 3;
    int mejor = rand() % poblacion.size();
    for (int i = 1; i < torneo; ++i) {
        int otro = rand() % poblacion.size();
        if (fitness[otro] > fitness[mejor]) {
            mejor = otro;
        }
    }
    return poblacion[mejor];
}

Cromosoma generarPoblacionInicial(vector<Pieza>& listaPiezas2, vector<Stock>& listaStocks2, int tamanoPoblacion) {
    vector<int> cromosomaTemp;
    Cromosoma cromosoma(numPiezas);
    
    Stock solucion;
    int numPiezaLista=0,indiceAleatorio=0,entra=0,menorH=100,a,b;
    int piezaEscogida,i=0,j=0,piezaPasada,ancho,alto,intentos=0,indice;
    double feromonas,heuristica,probabillidad=100,mProb=0;
    vector<int> indicePiezaEscogida;
    
    vector<Pieza> listaPiezas = listaPiezas2;
    vector<Stock> listaStocks = listaStocks2;
    
    for (const Pieza& pieza2 : listaPiezas) {
        if (listaPiezas.empty()) break;
        entra=0,i=0,intentos=0;
        while(intentos <= (numPiezas)){
            // Random por numero de piezas
            indiceAleatorio = rand() % (numPiezas-1);
            // Obtenemos el indice del id que queremos
            auto it = find_if(listaPiezas.begin(), listaPiezas.end(), 
                          [indiceAleatorio](const Pieza& pieza) {
                              return pieza.getID() == indiceAleatorio;
                          });
                          
            if (it != listaPiezas.end()) {
                indice = distance(listaPiezas.begin(), it);
                //listaPiezas[indice].imprimirPieza();
                //cout << "Ancho x Alto:" << anchoMayor << " , " << altoMayor<<endl;
            }           
            
            // Si entra la pieza la insertamos
            if (find(indicePiezaEscogida.begin(), indicePiezaEscogida.end(), indiceAleatorio) == indicePiezaEscogida.end()) {
                if (listaPiezas[indice].getW() + anchoMayor <= listaStocks[0].getW() && listaPiezas[indice].getH() <= listaStocks[0].getH()) {
                    indicePiezaEscogida.push_back(indiceAleatorio);
                    // Defino la longitud más grande a cortar en guillotina         
                    if(altoMayor>listaPiezas[indice].getH()){
                         anchoMayor += listaPiezas[indice].getW();
                    }else{
                        anchoMayor += listaPiezas[indice].getW();
                        altoMayor = listaPiezas[indice].getH();
                    }

                    // Elimino pieza inicial
                    auto it2 = std::find_if(listaPiezas.begin(), listaPiezas.end(), [indiceAleatorio](const Pieza& pieza) {
                        return pieza.getID() == indiceAleatorio;
                    });
                    if (it2 != listaPiezas.end()) {
                        listaPiezas.erase(it2);
                    }
                    matriz[i][j]=indiceAleatorio;
                    i++;
                }
            }          
            intentos++;
        }
        listaStocks[0].setH(listaStocks[0].getH()-altoMayor);
        altoMayor=0;
        anchoMayor=0;
        j++;
    }
    cromosomaTemp = convertirMatrizACromosoma(matriz);
    generarCromosoma(cromosoma,cromosomaTemp,listaPiezas2);
    
    return cromosoma;
}


void generarCromosoma(Cromosoma& cromosoma, const std::vector<int>& cromosomaTemp, const std::vector<Pieza>& listaPiezas) {
    // Recorremos la lista de IDs en cromosomaTemp
    for (int i = 0; i < cromosomaTemp.size(); ++i) {
        int piezaID = cromosomaTemp[i];
        
        // Detenemos el llenado si encontramos un -1
        if (piezaID == -1) {
            break;
        }

        // Buscamos la pieza correspondiente en la lista de piezas por ID
        auto it = std::find_if(listaPiezas.begin(), listaPiezas.end(), 
                               [piezaID](const Pieza& pieza) {
                                   return pieza.getID() == piezaID;
                               });

        if (it != listaPiezas.end()) {
            // Si encontramos la pieza, la insertamos en el cromosoma
            cromosoma.setGene(i, *it);
        } else {
            std::cerr << "Error: Pieza con ID " << piezaID << " no encontrada en la lista de piezas." << std::endl;
        }
    }
}


void algoritmoGA(vector<Pieza>& listaPiezas, vector<Stock>& listaStocks,int tamanoPoblacion,
        int generaciones, double tasaMutacion) {
        Poblacion poblacion;
        
    // Generamos la población inicial
    for (int i = 0; i < tamanoPoblacion; ++i) {
        generarPoblacionInicial(listaPiezas, listaStocks, tamanoPoblacion);
        poblacion.push_back(cromosoma);
    }
        
//    std::cout << "Población inicial:" << std::endl;
//    for (int i = 0; i < poblacion.size(); ++i) {
//        std::cout << "Cromosoma " << i + 1 << ": ";
//        for (int gen : poblacion[i]) {
//            std::cout << gen << " ";
//        }
//        std::cout << std::endl;
//    }

    for (int generacion = 0; generacion < generaciones; ++generacion) {
        vector<double> fitness;
        for (const auto& cromosoma : poblacion) {
            fitness.push_back(calcularFitness(cromosoma, listaStocks));
        }

        vector<vector<int>> nuevaPoblacion;

        // Crear nueva población mediante selección, cruce y mutación.
        while (nuevaPoblacion.size() < tamanoPoblacion) {
            vector<int> padre1 = seleccionar(poblacion, fitness);
            vector<int> padre2 = seleccionar(poblacion, fitness);

            // Corrección: Usamos make_pair en lugar de structured binding.
            pair<vector<int>, vector<int>> hijos = cruzar(padre1, padre2);
            vector<int> hijo1 = hijos.first;
            vector<int> hijo2 = hijos.second;

            mutar(hijo1,tasaMutacion);
            mutar(hijo2,tasaMutacion);

            nuevaPoblacion.push_back(hijo1);
            nuevaPoblacion.push_back(hijo2);
        }

        poblacion = nuevaPoblacion;
    }

    // Encontrar la mejor solución al final.
    vector<int> mejorSolucion = poblacion[0];
    double mejorFitness = calcularFitness(mejorSolucion, listaStocks);
    for (const auto& cromosoma : poblacion) {
        double fit = calcularFitness(cromosoma, listaStocks);
        if (fit > mejorFitness) {
            mejorFitness = fit;
            mejorSolucion = cromosoma;
        }
    }

    cout << "Mejor solución encontrada con fitness: " << mejorFitness << endl;
    for (int gene : mejorSolucion) {
        cout << gene << " ";
    }
    cout << endl; 
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));
    int numPiezas = 40, i=0; 
    int tamanoPoblacion = 10; 
    int generaciones = 50;
    double tasaMutacion = 0.1;
    
    vector<int> resultado;
    vector<Pieza> listaPiezas = generarListaPiezas(numPiezas);
    vector<Stock> listaStocks = generarListaStocks(1);

    sort(listaStocks.begin(), listaStocks.end(), compararStocks);
    sort(listaPiezas.begin(), listaPiezas.end(), compararPiezas);
    
    algoritmoGA(listaPiezas,listaStocks,tamanoPoblacion,generaciones,tasaMutacion);
    cout << "Ejecución del algoritmo GA finalizada." << endl;
    
//    cout << "Lista de Piezas:\n";
//    for (const Pieza& pieza : listaPiezas) {
//        pieza.imprimirPieza();
//    }
//    cout << "\nLista de Stock:\n";
//    listaStocks[0].imprimirStock();
//    
//   cout << endl;
//   
    return 0;
}

