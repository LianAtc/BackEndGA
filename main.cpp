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
#include "ClPoblacion.h"

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

void calcularDesperdicio(Cromosoma& cromosoma, const Stock& stock) {
    double areaTotalPiezas = 0;

    // Recorrer los genes del cromosoma (piezas) y sumar sus áreas.
    for (const Pieza& pieza : cromosoma.getGenes()) {
        areaTotalPiezas += pieza.calcularArea();
    }

    double areaStock = stock.calcularArea();
    double desperdicio = areaStock - areaTotalPiezas;

    // Asegurarse de que el desperdicio no sea negativo
    if (desperdicio < 0) {
        desperdicio = 0;
    }

    // Invertir el desperdicio para definir el fitness (menor desperdicio = mayor fitness)
    double fitness = 1.0 / (1.0 + desperdicio); // Ajuste de fitness: más alto con menos desperdicio.
    cromosoma.setFitness(fitness); // Configura el fitness del cromosoma.

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

Poblacion generarPoblacionInicial(vector<Pieza>& listaPiezas2, vector<Stock>& listaStocks2, int tamanoPoblacion) {
    Poblacion poblacion(tamanoPoblacion, numPiezas);
    
    Stock solucion;
    int numPiezaLista=0,indiceAleatorio=0,entra=0,menorH=100,a,b;
    int piezaEscogida,i=0,j=0,piezaPasada,ancho,alto,intentos=0,indice;
    double feromonas,heuristica,probabillidad=100,mProb=0;
    vector<int> indicePiezaEscogida;
    
    for (int n = 0; n < tamanoPoblacion; ++n) {
        Cromosoma cromosoma(listaPiezas2.size());
        vector<int> cromosomaTemp(numPiezas, -1);
    
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
        poblacion.setCromosoma(n, cromosoma);
    }
    
    return poblacion;
}

Cromosoma seleccionarRuleta(const Poblacion& poblacion) {
    double sumaFitness = 0.0;

    // Sumar todos los fitness
    for (const Cromosoma& cromosoma : poblacion.getCromosomas()) {
        sumaFitness += cromosoma.getFitness();
    }

    // Generar un número aleatorio entre 0 y sumaFitness
    double puntoAleatorio = (rand() / static_cast<double>(RAND_MAX)) * sumaFitness;
    double sumaParcial = 0.0;

    // Seleccionar el cromosoma
    for (const Cromosoma& cromosoma : poblacion.getCromosomas()) {
        sumaParcial += cromosoma.getFitness();
        if (sumaParcial >= puntoAleatorio) {
            return cromosoma;
        }
    }

    // Por seguridad, devolver el último cromosoma si el bucle no encuentra uno
    return poblacion.getCromosomas().back();
}

vector<Cromosoma> seleccionarElitista(const Poblacion& poblacion, int numElites) {
    vector<Cromosoma> elites;

    // Copiar cromosomas y ordenarlos por fitness
    vector<Cromosoma> cromosomas = poblacion.getCromosomas();
    sort(cromosomas.begin(), cromosomas.end(), 
         [](const Cromosoma& a, const Cromosoma& b) {
             return a.getFitness() > b.getFitness();
         });

    // Tomar los mejores numElites cromosomas
    for (int i = 0; i < numElites && i < cromosomas.size(); ++i) {
        elites.push_back(cromosomas[i]);
    }

    return elites;
}

void algoritmoGA(vector<Pieza>& listaPiezas, vector<Stock>& listaStocks,int tamanoPoblacion,
        int generaciones, double tasaMutacion) {
    
    Poblacion poblacion;
    poblacion = generarPoblacionInicial(listaPiezas,listaStocks,tamanoPoblacion);
    
    for (Cromosoma& cromosoma : poblacion.getCromosomas()) {
        calcularDesperdicio(cromosoma, listaStocks[0]);
    }

    for (int generacion = 0; generacion < generaciones; ++generacion) {
        Poblacion nuevaPoblacion;
        int numElites = 2; // Número de cromosomas élite que pasarán directamente a la nueva generación

        // Seleccionar élites
        vector<Cromosoma> elites = seleccionarElitista(poblacion, numElites);
        for (const Cromosoma& elite : elites) {
            nuevaPoblacion.addCromosoma(elite);
        }

        // Generar el resto de la nueva población mediante selección y cruce
        while (nuevaPoblacion.getCromosomas().size() < poblacion.getCromosomas().size()) {
            Cromosoma padre1 = seleccionarRuleta(poblacion);  // o seleccionarTorneo
            Cromosoma padre2 = seleccionarRuleta(poblacion);  // o seleccionarTorneo

//            auto [hijo1, hijo2] = cruzar(padre1, padre2);
//            mutar(hijo1, tasaMutacion);
//            mutar(hijo2, tasaMutacion);
//
//            nuevaPoblacion.addCromosoma(hijo1);
//            nuevaPoblacion.addCromosoma(hijo2);
        }
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

