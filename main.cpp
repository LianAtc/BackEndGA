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
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <stack>
#include <set>

#include "ClPieza.h"
#include "ClBase.h"

#include "ClCromosoma.h"
#include "ClPoblacion.h"

using namespace std;

int numPiezas = 90;
float anchoMayor,altoMayor;
float desTotal;

vector<Pieza> generarListaPiezas(int cantidad) {
    vector<Pieza> listaPiezas;
    vector<pair<float, float>> medidasDisponibles = {
        {45, 8.6}, {15, 7.5}, 
        {40, 25.0}, {40, 8.0},
        {40, 20.0}, {30, 15.0},
        {20, 10.0}, {40, 10.0},
        {30, 7.5}, {25, 12.0},
        {50, 9.5}, {35, 8.0},
        {60, 10.0}, {45, 12.5},
        {25, 5.0}, {55, 15.0},
        {65, 20.0}, {20, 7.0},
        {30, 5.5}, {40, 6.5},
        {50, 12.0}, {35, 10.0},
        {25, 8.0}, {45, 10.5},
        {55, 11.5}, {30, 20.0},
        {60, 15.0}, {35, 7.5},
        {20, 5.0}, {25, 10.0},
        {45, 8.6}, {15, 7.5}, 
        {40, 25.0}, {40, 8.0},
        {40, 20.0}, {30, 15.0},
        {20, 10.0}, {40, 10.0},
        {30, 7.5}, {25, 12.0},
        {50, 9.5}, {35, 8.0},
        {60, 10.0}, {45, 12.5},
        {25, 5.0}, {55, 15.0},
        {65, 20.0}, {20, 7.0},
        {30, 5.5}, {40, 6.5},
        {50, 12.0}, {35, 10.0},
        {25, 8.0}, {45, 10.5},
        {55, 11.5}, {30, 20.0},
        {60, 15.0}, {35, 7.5},
        {20, 5.0}, {25, 10.0},
        {45, 8.6}, {15, 7.5}, 
        {40, 25.0}, {40, 8.0},
        {40, 20.0}, {30, 15.0},
        {20, 10.0}, {40, 10.0},
        {30, 7.5}, {25, 12.0},
        {50, 9.5}, {35, 8.0},
        {60, 10.0}, {45, 12.5},
        {25, 5.0}, {55, 15.0},
        {65, 20.0}, {20, 7.0},
        {30, 5.5}, {40, 6.5},
        {50, 12.0}, {35, 10.0},
        {25, 8.0}, {45, 10.5},
        {55, 11.5}, {30, 20.0},
        {60, 15.0}, {35, 7.5},
        {20, 5.0}, {25, 10.0}
    };

    for (int i = 0; i < cantidad; ++i) {
        float x = 0;
        float y = 0;
        bool rotada = 0;

        int indiceAleatorio = rand() % medidasDisponibles.size();
        float w = medidasDisponibles[i].first;
        float h = medidasDisponibles[i].second;
        
        Pieza p(i, x, y, w, h, rotada);
        listaPiezas.push_back(p);
    }
    return listaPiezas;
}

vector<Stock> generarListaStocks(int cantidad) {
    vector<Stock> listaStocks;
    
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < cantidad; ++i) {
        int w, h;
        
        if (rand() % 2 == 0) {
            w = 60;
            h = 60;
        } else {
            w = 60;
            h = 60;
        }
        
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
    return (a.getW() * a.getH()) > (b.getW() * b.getH());
}

void ordenarStocks(vector<Stock>& stocks) {
    sort(stocks.begin(), stocks.end(), compararStocks);
}

void calcularDesperdicio(Cromosoma& cromosoma, const Stock& stock) {
    double areaTotalPiezas = 0;

    for (const Pieza& pieza : cromosoma.getGenes()) {
        areaTotalPiezas += pieza.calcularArea();
    }
    double areaStock = stock.calcularArea();
    double desperdicio = areaTotalPiezas/areaStock;

    if (desperdicio < 0) {
        desperdicio = 0;
    }
    double fitness = (1.0 - desperdicio);
    cromosoma.setFitness(fitness); 
}

vector<int> obtenerListaIDs(const std::vector<Pieza>& listaPiezas) {
    std::vector<int> listaIDs;
    for (const auto& pieza : listaPiezas) {
        listaIDs.push_back(pieza.getID());
    }
    return listaIDs;
}

double calcularHeuristica(Stock& stock, Pieza& pieza) {
    double desperdicio, areaUsada, areaPiezas;
    double desperdicioOriginal = 1;
    double desperdicioRotado = 1;

    if (pieza.getH() <= stock.getH() && anchoMayor + pieza.getW() <= stock.getW()) {
        areaUsada = (altoMayor > pieza.getH()) ? 
            (altoMayor * (anchoMayor + pieza.getW())) : 
            (pieza.getH() * (anchoMayor + pieza.getW()));

        areaPiezas = (anchoMayor * altoMayor) + (pieza.getW() * pieza.getH());
        desperdicioOriginal = (areaUsada - areaPiezas) / areaUsada;
    }

    pieza.rotarPieza();
    if (pieza.getH() <= stock.getH() && anchoMayor + pieza.getW() <= stock.getW()) {
        areaUsada = (altoMayor > pieza.getH()) ? 
            (altoMayor * (anchoMayor + pieza.getW())) : 
            (pieza.getH() * (anchoMayor + pieza.getW()));

        areaPiezas = (anchoMayor * altoMayor) + (pieza.getW() * pieza.getH());
        desperdicioRotado = (areaUsada - areaPiezas) / areaUsada;
    }

    if (desperdicioRotado < desperdicioOriginal) {
        if (desperdicioRotado == 0) return 0;    
        return desperdicioRotado;
    } else {
        pieza.rotarPieza();
        return desperdicioOriginal;
    }
}

void imprimirVector(const std::vector<int>& indicePiezaEscogida) {
    for (int i = 0; i < indicePiezaEscogida.size(); i++) {
        if(indicePiezaEscogida[i] == -1) break;
        cout <<indicePiezaEscogida[i]<<" ";
    }
    cout<<endl;
}

Cromosoma convertirVectorACromosoma(const vector<int>& vec, int tamanoCromosoma, const vector<Pieza>& listaPiezas) {
    Cromosoma cromosoma(tamanoCromosoma);

    for (int i = 0; i < vec.size() && i < tamanoCromosoma; ++i) {
        int id = vec[i];
        auto it = find_if(listaPiezas.begin(), listaPiezas.end(), [id](const Pieza& pieza) {
            return pieza.getID() == id;
        });

        if (it != listaPiezas.end()) {
            cromosoma.setGene(i, *it);
        } else {
            cromosoma.setGene(i, Pieza(-1, 0, 0, 0, 0, false));
        }
    }

    for (int i = vec.size(); i < tamanoCromosoma; ++i) {
        cromosoma.setGene(i, Pieza(-1, 0, 0, 0, 0, false));
    }

    return cromosoma;
}

Poblacion generarPoblacionInicial(vector<Pieza>& listaPiezas2, vector<Stock>& listaStocks2, int tamanoPoblacion) {
    Poblacion poblacion(tamanoPoblacion, numPiezas);
    
    Stock solucion;
    int numPiezaLista=0,indiceAleatorio=0,entra=0,a,b,idAleatorio;
    int piezaEscogida,piezaPasada,intentos=0,indice;
    float feromonas,heuristica,probabillidad=100,mProb=0,menorH=1,ancho,alto;
    vector<int> indicePiezaEscogida;
    
    for (int n = 0; n < tamanoPoblacion; ++n) {
        
        Cromosoma cromosoma(listaPiezas2.size());
    
        vector<Pieza> listaPiezas = listaPiezas2;
        vector<Stock> listaStocks = listaStocks2;
        
        altoMayor=0, anchoMayor=0;
        indicePiezaEscogida.clear();

        for (const Pieza& pieza2 : listaPiezas) {
            if (listaPiezas.empty()) break;
            entra=0,intentos=0;
            while(entra==0 && intentos < (numPiezas-1)){ 
                auto listaIDs = obtenerListaIDs(listaPiezas);
                idAleatorio = rand() % listaIDs.size();
                indiceAleatorio = listaIDs[idAleatorio];

                auto it = find_if(listaPiezas.begin(), listaPiezas.end(), 
                              [indiceAleatorio](const Pieza& pieza) {
                                  return pieza.getID() == indiceAleatorio;
                              });
                              
                if (it != listaPiezas.end()) {
                    indice = distance(listaPiezas.begin(), it);
                    //listaPiezas[indice].imprimirPieza();
                }           


                if (find(indicePiezaEscogida.begin(), indicePiezaEscogida.end(), indiceAleatorio) == indicePiezaEscogida.end()) {
                    if (listaPiezas[indice].getW() + anchoMayor <= listaStocks[0].getW() && listaPiezas[indice].getH() <= listaStocks[0].getH()) {
                        entra=1;
                        indicePiezaEscogida.push_back(indiceAleatorio);
                    }else{
                        entra=0;
                    }
                }          
                intentos++;
            }
            if(entra==0) break;
            intentos=0;
            
            anchoMayor = listaPiezas[indice].getW();
            altoMayor = listaPiezas[indice].getH();
            
            //cout << "Ancho x Alto:" << anchoMayor << " , " << altoMayor<<endl;  
            
            // Elimino pieza inicial
            auto it2 = std::find_if(listaPiezas.begin(), listaPiezas.end(), [indiceAleatorio](const Pieza& pieza) {
                return pieza.getID() == indiceAleatorio;
            });
            if (it2 != listaPiezas.end()) {
                listaPiezas.erase(it2);
            }
            
            piezaPasada = indiceAleatorio;
            
            menorH=1,mProb=1;
            
            for (Pieza& pieza2 : listaPiezas) {
                for (Pieza& pieza : listaPiezas) {
                    probabillidad = calcularHeuristica(listaStocks[0],pieza);
                    if(mProb >= probabillidad){    
                        mProb = probabillidad;
                        piezaEscogida = pieza.getID();
                        alto = pieza.getH();
                        ancho = pieza.getW();
                    }
                }
                if (mProb == 1) break;
                //cout << "Pieza acompañante: " << piezaEscogida<<endl;
                
                if(altoMayor>alto){
                    anchoMayor += ancho;
                }else{
                    anchoMayor += ancho;
                    altoMayor = alto;
                }
                
                indicePiezaEscogida.push_back(piezaEscogida);
                piezaPasada = piezaEscogida;
                
                // Elimino pieza escogida
                auto it = std::find_if(listaPiezas.begin(), listaPiezas.end(), [piezaEscogida](const Pieza& pieza) {
                    return pieza.getID() == piezaEscogida;
                });
                if (it != listaPiezas.end()) {
                    listaPiezas.erase(it);
                }
                //desTotal+=heuristica;
                mProb=0;piezaEscogida=-1;menorH=1;
            }
            listaStocks[0].setH(listaStocks[0].getH()-altoMayor);
           // cout << "Nuevo Alto: " << listaStocks[0].getH()<<endl;
            altoMayor=0;
            anchoMayor=0;
        }
        cromosoma =  convertirVectorACromosoma(indicePiezaEscogida,numPiezas,listaPiezas2);
        poblacion.setCromosoma(n, cromosoma);
        
//        cout<<"Impresion Vector"<<endl;
//        imprimirVector(indicePiezaEscogida);
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

    vector<Cromosoma> cromosomas = poblacion.getCromosomas();
    sort(cromosomas.begin(), cromosomas.end(), 
         [](const Cromosoma& a, const Cromosoma& b) {
             return a.getFitness() < b.getFitness();
         });

    for (int i = 0; i < numElites && i < cromosomas.size(); ++i) {
        // unu
        elites.push_back(cromosomas[i]);
    }

    return elites;
}

pair<Cromosoma, Cromosoma> cruzar(const Cromosoma& padre1, const Cromosoma& padre2, double probCruce) {
    
    double probabilidad = static_cast<double>(rand()) / RAND_MAX;
    if (probabilidad > probCruce) {
        return make_pair(padre1, padre2); // Se devuelven los padres sin cambios
    }
    
    // Ignorar valores dummy (-1)
    int tamanoPadre1 = 0;
    for (const Pieza& gen : padre1.getGenes()) {
        if (gen.getID() == -1) break;
        ++tamanoPadre1;
    }

    int tamanoPadre2 = 0;
    for (const Pieza& gen : padre2.getGenes()) {
        if (gen.getID() == -1) break;
        ++tamanoPadre2;
    }

    int tamanoCruce = min(tamanoPadre1, tamanoPadre2);
    //cout << "Tamaño numérico usado para el cruce: " << tamanoCruce << endl;

    Cromosoma hijo1 = padre1;
    Cromosoma hijo2 = padre2;

    if (tamanoCruce == 0) {
        //cout << "No hay parte numérica para realizar el cruce. Se devuelven los padres sin cambios." << endl;
        return make_pair(hijo1, hijo2);
    }

    // Seleccionar puntos de cruce aleatorios
    int punto1 = rand() % tamanoCruce;
    int punto2 = rand() % tamanoCruce;
    if (punto1 > punto2) swap(punto1, punto2);
    //cout << "Punto de cruce 1: " << punto1 << ", Punto de cruce 2: " << punto2 << endl;

    // Intercambio de genes en el rango seleccionado
    for (int i = punto1; i <= punto2; ++i) {
        Pieza genPadre1 = padre1.getGene(i);
        Pieza genPadre2 = padre2.getGene(i);
        hijo1.setGene(i, genPadre2);
        hijo2.setGene(i, genPadre1);
    }

    // Manejo de duplicados en hijo1
    unordered_map<int, int> frecuenciaHijo1;
    for (const Pieza& gen : hijo1.getGenes()) {
        if (gen.getID() != -1) {
            frecuenciaHijo1[gen.getID()]++;
        }
    }

    for (int i = 0; i < tamanoCruce; ++i) {
        if (i < punto1 || i > punto2) {
            Pieza genPadre = hijo1.getGene(i);
            if (frecuenciaHijo1[genPadre.getID()] > 1) {  // Verifica si el gen está duplicado
                for (const Pieza& genReemplazo : padre2.getGenes()) {
                    if (genReemplazo.getID() != -1 && frecuenciaHijo1[genReemplazo.getID()] == 0) {
                        hijo1.setGene(i, genReemplazo);
                        frecuenciaHijo1[genPadre.getID()]--;
                        frecuenciaHijo1[genReemplazo.getID()]++;
                        break;
                    }
                }
            }
        }
    }

    // Manejo de duplicados en hijo2
    unordered_map<int, int> frecuenciaHijo2;
    for (const Pieza& gen : hijo2.getGenes()) {
        if (gen.getID() != -1) {
            frecuenciaHijo2[gen.getID()]++;
        }
    }

    for (int i = 0; i < tamanoCruce; ++i) {
        if (i < punto1 || i > punto2) {
            Pieza genPadre = hijo2.getGene(i);
            if (frecuenciaHijo2[genPadre.getID()] > 1) {  // Verifica si el gen está duplicado
                for (const Pieza& genReemplazo : padre1.getGenes()) {
                    if (genReemplazo.getID() != -1 && frecuenciaHijo2[genReemplazo.getID()] == 0) {
                        hijo2.setGene(i, genReemplazo);
                        frecuenciaHijo2[genPadre.getID()]--;
                        frecuenciaHijo2[genReemplazo.getID()]++;
                        break;
                    }
                }
            }
        }
    }

    return make_pair(hijo1, hijo2);
}

void mutar(Cromosoma& cromosoma, double probMutacion) {
    double probabilidad = static_cast<double>(rand()) / RAND_MAX;

    if (probabilidad < probMutacion) {
        int tamano = cromosoma.getGenes().size();
        
        // Collect indices of valid genes (non -1)
        vector<int> validIndices;
        for (int i = 0; i < tamano; ++i) {
            if (cromosoma.getGene(i).getID() == -1) break;
            validIndices.push_back(i);
        }

        // Only mutate if there are at least two valid indices
        if (validIndices.size() > 1) {
            // Randomly pick two distinct indices from valid indices
            int index1 = validIndices[rand() % validIndices.size()];
            int index2 = validIndices[rand() % validIndices.size()];
            while (index2 == index1) {
                index2 = validIndices[rand() % validIndices.size()];
            }

            // Debug print to see selected indices
//            cout << "Mutating genes at positions " << index1 << " and " << index2 << endl;

            // Perform the mutation by swapping the genes
            Pieza temp = cromosoma.getGene(index1);
            cromosoma.setGene(index1, cromosoma.getGene(index2));
            cromosoma.setGene(index2, temp);

        }
    }
}

void colocarPiezasEnOrden(Cromosoma& cromosoma, Stock& stock) {
    float anchoMaximo = stock.getW();
    float altoMaximo = stock.getH();
    float anchoFilaActual = 0;
    float alturaFilaActual = 0;
    float alturaTotal = 0;

    cout << "Colocación de las piezas en el stock (" << anchoMaximo << "x" << altoMaximo << "):" << endl;

    for (Pieza& pieza : cromosoma.getGenes()) {
        if (pieza.getID() == -1) break; // Ignorar posiciones vacías

        // Verificar si la pieza cabe en la fila actual
        if (anchoFilaActual + pieza.getW() <= anchoMaximo) {
            // Colocar la pieza en la fila actual y actualizar su posición
            pieza.setX(anchoFilaActual);
            pieza.setY(alturaTotal);

            cout << "Pieza ID: " << pieza.getID() << " colocada en (" << pieza.getX() 
                    << ", " << pieza.getY() << "), Tamaño: " << pieza.getW() << "x" << pieza.getH() 
                    << " , Rotada: "<< pieza.getRot() <<endl;
            anchoFilaActual += pieza.getW();
            alturaFilaActual = max(alturaFilaActual, pieza.getH());
        } else {
            // Mover a la siguiente fila
            alturaTotal += alturaFilaActual;
            if (alturaTotal + pieza.getH() > altoMaximo) {
//                cout << "La pieza ID: " << pieza.getID() << " no cabe en el stock restante. Colocación terminada." << endl;
                break;
            }

            // Colocar la pieza en la nueva fila y actualizar su posición
            pieza.setX(0);
            pieza.setY(alturaTotal);

            cout << "Pieza ID: " << pieza.getID() << " colocada en (" << pieza.getX() << ", " 
                    << pieza.getY() << "), Tamaño: " << pieza.getW() << "x" << pieza.getH() 
                    << " , Rotada: "<< pieza.getRot() <<endl;
            anchoFilaActual = pieza.getW();
            alturaFilaActual = pieza.getH();
        }
    }

//    cout << "Colocación finalizada." << endl;
}

bool verificarCromosoma(Cromosoma& cromosoma, const Stock& stock) {
    float anchoMaximo = stock.getW();
    float altoMaximo = stock.getH();
    float anchoFilaActual = 0;
    float alturaFilaActual = 0;
    float alturaTotal = 0;

    for (const Pieza& pieza : cromosoma.getGenes()) {
        if (pieza.getID() == -1) break; // Ignorar posiciones vacías

        // Verificar si la pieza cabe en la fila actual antes de actualizar el ancho
        if (anchoFilaActual + pieza.getW() <= anchoMaximo) {
            // Colocar la pieza en la fila actual
            anchoFilaActual += pieza.getW();
            alturaFilaActual = max(alturaFilaActual, pieza.getH());
        } else {
            // Mover a la siguiente fila y verificar si la nueva fila cabe en el alto total
            alturaTotal += alturaFilaActual;
            if (alturaTotal + pieza.getH() > altoMaximo) {
                return false; // No cabe en el stock
            }
            
            // Reiniciar los valores para la nueva fila
            anchoFilaActual = pieza.getW();
            alturaFilaActual = pieza.getH();
        }
    }

    // Verificar la altura total al final
    alturaTotal += alturaFilaActual;
    if (alturaTotal > altoMaximo) {
        return false; // No cabe en el stock
    }

    calcularDesperdicio(cromosoma, stock);
    if (cromosoma.getFitness() < 0.0) {
        cout << "Desperdicio en la función: " << cromosoma.getFitness() << endl;
    }

    // Si todas las piezas encajan, la solución es válida
    return true;
}

void algoritmoGA(vector<Pieza>& listaPiezas, vector<Stock>& listaStocks, int tamanoPoblacion,
                 int generaciones, double probMutacion, double probCruce, int numElites) {
    // Variables para rastrear el mejor cromosoma global
    Cromosoma mejorCromosomaGlobal;
    double mejorFitnessGlobal = std::numeric_limits<double>::max();

    // Generar la población inicial
    Poblacion poblacion = generarPoblacionInicial(listaPiezas, listaStocks, tamanoPoblacion);

    // Evaluar el fitness inicial y determinar el mejor cromosoma
    for (Cromosoma& cromosoma : poblacion.getCromosomas()) {
        calcularDesperdicio(cromosoma, listaStocks[0]);
        if (cromosoma.getFitness() < mejorFitnessGlobal) {
            mejorFitnessGlobal = cromosoma.getFitness();
            mejorCromosomaGlobal = cromosoma;
        }
    }

    // Bucle principal de generaciones
    for (int generacion = 0; generacion < generaciones; ++generacion) {
        Poblacion nuevaPoblacion;

        // Seleccionar élites y agregarlas a la nueva población
        vector<Cromosoma> elites = seleccionarElitista(poblacion, numElites);
        for (const Cromosoma& elite : elites) {
            nuevaPoblacion.addCromosoma(elite);
        }

        // Generar nueva población mediante cruces y mutaciones
        while (nuevaPoblacion.getCromosomas().size() < tamanoPoblacion) {
            Cromosoma padre1 = seleccionarRuleta(poblacion);
            Cromosoma padre2;

            do {
                padre2 = seleccionarRuleta(poblacion);
            } while (padre1 == padre2);

            // Generar hijos mediante cruce
            pair<Cromosoma, Cromosoma> hijos = cruzar(padre1, padre2, probCruce);
            Cromosoma hijo1 = hijos.first;
            Cromosoma hijo2 = hijos.second;

            // Verificar y calcular fitness de los hijos
            if (verificarCromosoma(hijo1, listaStocks[0])) {
                calcularDesperdicio(hijo1, listaStocks[0]);
                nuevaPoblacion.addCromosoma(hijo1);
            }
            if (verificarCromosoma(hijo2, listaStocks[0])) {
                calcularDesperdicio(hijo2, listaStocks[0]);
                nuevaPoblacion.addCromosoma(hijo2);
            }
        }

        // Aplicar mutaciones y calcular fitness de la nueva población
        for (Cromosoma& cromosoma : nuevaPoblacion.getCromosomas()) {
            Cromosoma cromosomaOriginal = cromosoma;
            mutar(cromosoma, probMutacion);
            if (!verificarCromosoma(cromosoma, listaStocks[0])) {
                cromosoma = cromosomaOriginal;
            }
            calcularDesperdicio(cromosoma, listaStocks[0]);

            // Actualizar el mejor cromosoma global si es necesario
            if (cromosoma.getFitness() < mejorFitnessGlobal) {
                mejorFitnessGlobal = cromosoma.getFitness();
                mejorCromosomaGlobal = cromosoma;
            }
        }

        // Actualizar la población con la nueva generación
        poblacion = nuevaPoblacion;

        // Imprimir progreso
//        if (generacion % 10 == 0 || generacion == generaciones - 1) {
//            cout << "Generación " << generacion << ": Mejor fitness global = " << mejorFitnessGlobal << endl;
//        }
    }

    // Imprimir el mejor cromosoma global al final del algoritmo
//    cout << "Mejor cromosoma global encontrado:" << endl;
//    mejorCromosomaGlobal.imprimir();
    cout << mejorFitnessGlobal << endl;
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));
    int i=0; 
    int tamanoPoblacion = 500; 
    int generaciones = 50;
    int numElites=50;
    double probMutacion = 0.7, probCruce = 0.5;
    
    vector<int> resultado;
    vector<Pieza> listaPiezas = generarListaPiezas(numPiezas);
    vector<Stock> listaStocks = generarListaStocks(1);

    cout << "Lista de Piezas:\n";
    for (const Pieza& pieza : listaPiezas) {
        pieza.imprimirPieza();
    }
    cout << "\nLista de Stock:\n";
    listaStocks[0].imprimirStock();
    cout << endl;
    
    for(int i=0;i<=29;i++){
        algoritmoGA(listaPiezas,listaStocks,tamanoPoblacion,generaciones,
            probMutacion,probCruce, numElites);
    }
    cout << "Ejecución del algoritmo GA finalizada." << endl;
   
    return 0;
}

