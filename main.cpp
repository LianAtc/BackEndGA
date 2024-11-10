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
    
    for (int elemento : lista) {
        cout << elemento << " ";
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
    double desperdicio = areaTotalPiezas/areaStock;

    // Asegurarse de que el desperdicio no sea negativo
    if (desperdicio < 0) {
        desperdicio = 0;
    }

    // Invertir el desperdicio para definir el fitness (menor desperdicio = mayor fitness)
    double fitness = 100 * (1.0 - desperdicio); // Ajuste de fitness: más alto con menos desperdicio.
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

vector<int> obtenerListaIDs(const std::vector<Pieza>& listaPiezas) {
    std::vector<int> listaIDs;
    for (const auto& pieza : listaPiezas) {
        listaIDs.push_back(pieza.getID());
    }
    return listaIDs;
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
        
        altoMayor=0, anchoMayor=0, i=0, j=0;
        indicePiezaEscogida.clear();

        for (const Pieza& pieza2 : listaPiezas) {
            if (listaPiezas.empty()) break;
            entra=0,i=0,intentos=0;
            while(intentos <= (numPiezas)){
                // Random por numero de piezas existentes
                if (listaPiezas.empty()) break;
                auto listaIDs = obtenerListaIDs(listaPiezas);
                int idAleatorio = rand() % listaIDs.size();
                int indiceAleatorio = listaIDs[idAleatorio];
                // Obtenemos el indice del id que queremos
                auto it = find_if(listaPiezas.begin(), listaPiezas.end(), 
                              [indiceAleatorio](const Pieza& pieza) {
                                  return pieza.getID() == indiceAleatorio;
                              });

                if (it != listaPiezas.end()) {
                    indice = distance(listaPiezas.begin(), it);
//                    listaPiezas[indice].imprimirPieza();
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
        //cout<<"Impresión cromosoma"<<endl;
        //cromosoma.imprimir();
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
             return a.getFitness() < b.getFitness();
         });

    // Tomar los mejores numElites cromosomas
    for (int i = 0; i < numElites && i < cromosomas.size(); ++i) {
        elites.push_back(cromosomas[i]);
    }

    return elites;
}

pair<Cromosoma, Cromosoma> cruzar(const Cromosoma& padre1, const Cromosoma& padre2) {
    int tamano = padre1.getGenes().size();
    Cromosoma hijo1(tamano);
    Cromosoma hijo2(tamano);
    set<int> genesUsadosHijo1;
    set<int> genesUsadosHijo2;

    // Seleccionar un punto de cruce aleatorio
    int puntoCruce = rand() % tamano;

    // Copiar genes de los padres a los hijos antes del punto de cruce
    for (int i = 0; i < puntoCruce; ++i) {
        hijo1.setGene(i, padre1.getGene(i));
        hijo2.setGene(i, padre2.getGene(i));
        genesUsadosHijo1.insert(padre1.getGene(i).getID());
        genesUsadosHijo2.insert(padre2.getGene(i).getID());
    }

    // Completar los hijos después del punto de cruce sin duplicados
    for (int i = puntoCruce; i < tamano; ++i) {
        if (genesUsadosHijo1.find(padre2.getGene(i).getID()) == genesUsadosHijo1.end()) {
            hijo1.setGene(i, padre2.getGene(i));
            genesUsadosHijo1.insert(padre2.getGene(i).getID());
        } else {
            // Encontrar un gen único de padre1
            for (const Pieza& gen : padre1.getGenes()) {
                if (genesUsadosHijo1.find(gen.getID()) == genesUsadosHijo1.end()) {
                    hijo1.setGene(i, gen);
                    genesUsadosHijo1.insert(gen.getID());
                    break;
                }
            }
        }

        if (genesUsadosHijo2.find(padre1.getGene(i).getID()) == genesUsadosHijo2.end()) {
            hijo2.setGene(i, padre1.getGene(i));
            genesUsadosHijo2.insert(padre1.getGene(i).getID());
        } else {
            // Encontrar un gen único de padre2
            for (const Pieza& gen : padre2.getGenes()) {
                if (genesUsadosHijo2.find(gen.getID()) == genesUsadosHijo2.end()) {
                    hijo2.setGene(i, gen);
                    genesUsadosHijo2.insert(gen.getID());
                    break;
                }
            }
        }
    }

    return make_pair(hijo1, hijo2);
}

void mutar(Cromosoma& cromosoma, double tasaMutacion) {
    // Generar un número aleatorio entre 0 y 1 para decidir si se muta
    double probabilidad = static_cast<double>(rand()) / RAND_MAX;

    // Si la probabilidad es menor que la tasa de mutación, realiza la mutación
    if (probabilidad < tasaMutacion) {
        int tamano = cromosoma.getGenes().size();

        // Verificar que el tamaño del cromosoma permita la mutación
        if (tamano > 1) {
            // Elegir dos índices aleatorios diferentes para intercambiar
            int indice1 = rand() % tamano;
            int indice2 = rand() % tamano;

            // Asegurarse de que los índices sean distintos
            while (indice2 == indice1) {
                indice2 = rand() % tamano;
            }

            // Imprimir información sobre la mutación
//            cout << "Mutación: intercambiando genes en las posiciones " << indice1 << " y " << indice2 << endl;
//            cout << "Antes de la mutación:" << endl;
//            cromosoma.imprimir();

            // Intercambiar los genes
            Pieza temp = cromosoma.getGene(indice1);
            cromosoma.setGene(indice1, cromosoma.getGene(indice2));
            cromosoma.setGene(indice2, temp);

            //cout << "Después de la mutación:" << endl;
//            cromosoma.imprimir();
        }
    }
}

void verificarColocacionCromosoma(const Cromosoma& cromosoma, const Stock& stock) {
    int anchoActual = 0;
    int alturaActual = 0;
    int alturaFila = 0;
    const int anchoMaximo = stock.getAncho();
    const int altoMaximo = stock.getAlto();

    cout << "Colocación de las piezas en el stock (" << anchoMaximo << "x" << altoMaximo << "):" << endl;

    for (const Pieza& pieza : cromosoma.getGenes()) {
        if (pieza.getID() == -1) break;  // Detener si el cromosoma tiene una pieza vacía (ID 0)

        // Verificar si la pieza cabe en la fila actual
        if (anchoActual + pieza.getW() <= anchoMaximo) {
            // Colocar la pieza en la fila actual
            cout << "Pieza ID: " << pieza.getID() << " colocada en (" << anchoActual << ", " << alturaActual << "), Tamaño: " << pieza.getW() << "x" << pieza.getH() << endl;
            anchoActual += pieza.getW();
            alturaFila = max(alturaFila, static_cast<int>(pieza.getH()));  // Actualizar la altura de la fila según la pieza más alta
        } else {
            // Mover a la siguiente fila
            alturaActual += alturaFila;
            if (alturaActual + pieza.getH() > altoMaximo) {
                cout << "La pieza ID: " << pieza.getID() << " no cabe en el stock restante. Colocación terminada." << endl;
                break;
            }

            // Colocar la pieza en la nueva fila
            cout << "Pieza ID: " << pieza.getID() << " colocada en (0, " << alturaActual << "), Tamaño: " << pieza.getW() << "x" << pieza.getH() << endl;
            anchoActual = pieza.getW();
            alturaFila = pieza.getH();  // Nueva altura de la fila
        }
    }

    cout << "Colocación finalizada." << endl;
}


void algoritmoGA(vector<Pieza>& listaPiezas, vector<Stock>& listaStocks,int tamanoPoblacion,
        int generaciones, double tasaMutacion) {
    
    // Generamos la población inicial
    Poblacion poblacion;
    poblacion = generarPoblacionInicial(listaPiezas,listaStocks,tamanoPoblacion);
    
    // Calculamos el fitnees de cada solución para elegir las mejores
    for (Cromosoma& cromosoma : poblacion.getCromosomas()) {
        calcularDesperdicio(cromosoma, listaStocks[0]);
    }
    poblacion.imprimir();
    
    for (int generacion = 0; generacion < generaciones; ++generacion) {
        Poblacion nuevaPoblacion;
        int numElites = 2;

        // Seleccionar élites
        vector<Cromosoma> elites = seleccionarElitista(poblacion, numElites);
        for (const Cromosoma& elite : elites) {
            nuevaPoblacion.addCromosoma(elite);
        }

        // Generar el resto de la nueva población mediante selección y cruce
        while (nuevaPoblacion.getCromosomas().size() < poblacion.getCromosomas().size()) {
            Cromosoma padre1 = seleccionarRuleta(poblacion);
            Cromosoma padre2 = seleccionarRuleta(poblacion);
            
//            cout << "Padre 1:" << endl;
//            padre1.imprimir();
//            cout << "Padre 2:" << endl;
//            padre2.imprimir();
            
            calcularDesperdicio(padre1, listaStocks[0]);
            calcularDesperdicio(padre2, listaStocks[0]);

            pair<Cromosoma, Cromosoma> hijos = cruzar(padre1, padre2);
            Cromosoma hijo1 = hijos.first;
            Cromosoma hijo2 = hijos.second;
            
            calcularDesperdicio(hijo1, listaStocks[0]);
            calcularDesperdicio(hijo2, listaStocks[0]);
            
//            cout << "Hijo 1:" << endl;
//            hijo1.imprimir();
//            cout << "Hijo 2:" << endl;
//            hijo2.imprimir();
            
            mutar(hijo1, tasaMutacion);
            mutar(hijo2, tasaMutacion);

            nuevaPoblacion.addCromosoma(hijo1);
            nuevaPoblacion.addCromosoma(hijo2);
        }
    }
    cout << "La mejor solución de la población actual es:" << endl;
    Cromosoma mejorCromosoma = poblacion.getBestCromosoma();
    mejorCromosoma.imprimir();
    cout << endl; 
    verificarColocacionCromosoma(mejorCromosoma, listaStocks[0]);
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));
    int i=0; 
    int tamanoPoblacion = 10; 
    int generaciones = 100;
    double tasaMutacion = 0.1;
    
    vector<int> resultado;
    vector<Pieza> listaPiezas = generarListaPiezas(numPiezas);
    vector<Stock> listaStocks = generarListaStocks(1);

    sort(listaStocks.begin(), listaStocks.end(), compararStocks);
    sort(listaPiezas.begin(), listaPiezas.end(), compararPiezas);
    
    cout << "Lista de Piezas:\n";
    for (const Pieza& pieza : listaPiezas) {
        pieza.imprimirPieza();
    }
    cout << "\nLista de Stock:\n";
    listaStocks[0].imprimirStock();
   cout << endl;
    
    algoritmoGA(listaPiezas,listaStocks,tamanoPoblacion,generaciones,tasaMutacion);
    cout << "Ejecución del algoritmo GA finalizada." << endl;
   
    return 0;
}

