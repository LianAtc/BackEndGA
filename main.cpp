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

int numPiezas = 15;
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
    
//    for (int elemento : lista) {
//        cout << elemento << " ";
//    }
    
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
    vector<pair<float, float>> medidasDisponibles = {
        {45, 8.6}, {15, 7.5}
    };

    for (int i = 0; i < cantidad; ++i) {
        float x = 0;
        float y = 0;
        bool rotada = 0;

        // Elegir un par de medidas aleatoriamente
        int indiceAleatorio = rand() % medidasDisponibles.size();
        float w = medidasDisponibles[indiceAleatorio].first;
        float h = medidasDisponibles[indiceAleatorio].second;
        
        Pieza p(i, x, y, w, h, rotada);
        listaPiezas.push_back(p);
    }
    return listaPiezas;
}

vector<Stock> generarListaStocks(int cantidad) {
    vector<Stock> listaStocks;
    
    // Inicializar la semilla aleatoria
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < cantidad; ++i) {
        int w, h;
        
        // Elegir aleatoriamente entre las dos opciones de tamaño
        if (rand() % 2 == 0) {
            w = 60;
            h = 60;
        } else {
            w = 45;
            h = 45;
        }
        
        Stock s(w, h); // Crear el objeto Stock con las dimensiones seleccionadas
        listaStocks.push_back(s); // Agregar a la lista
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

double calcularHeuristica(const Stock& stock, const Pieza& pieza) {
    double desperdicio;

    if(pieza.getH()>stock.getH()) return -1;
    if(anchoMayor+pieza.getW()<= stock.getW()){
        if(altoMayor>pieza.getH()){
            desperdicio = (((altoMayor)*(anchoMayor+pieza.getW())) 
                    - ((anchoMayor*altoMayor)+
                    (pieza.getW()*pieza.getH()))) 
                    / ((altoMayor)*(anchoMayor+
                    pieza.getW()));
        }
        else{
            desperdicio = (((pieza.getH())*(anchoMayor+pieza.getW()) 
                    - (anchoMayor*altoMayor)+
                    (pieza.getW()* pieza.getH())))
                    / ((pieza.getH())*
                    (anchoMayor+pieza.getW()));
        }
    }else{
        return -1;
    }
    
    return desperdicio;
}


Poblacion generarPoblacionInicial(vector<Pieza>& listaPiezas2, vector<Stock>& listaStocks2, int tamanoPoblacion) {
    Poblacion poblacion(tamanoPoblacion, numPiezas);
    
    Stock solucion;
    int numPiezaLista=0,indiceAleatorio=0,entra=0,menorH=100,a,b,idAleatorio;
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
                    //cout << "Ancho x Alto:" << anchoMayor << " , " << altoMayor<<endl;
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
            
            matriz[i][j]=indiceAleatorio;
            i++;
            piezaPasada = indiceAleatorio;
            
            for (const Pieza& pieza2 : listaPiezas) {
                for (const Pieza& pieza : listaPiezas) {
                    heuristica = calcularHeuristica(listaStocks[0],pieza);
                    probabillidad = heuristica;
                    if(mProb<=probabillidad){    
                        mProb = probabillidad;
                        piezaEscogida = pieza.getID();
                        alto = pieza.getH();
                        ancho = pieza.getW();
                    }
                    if( heuristica != -1 ) menorH = heuristica;
                    //pieza.imprimirPieza();
                }
                if (menorH == -1 || menorH == 100) break;
                matriz[i][j]=piezaEscogida;
                i++;
                
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
                mProb=0;piezaEscogida=-1;menorH=100;
            }
            listaStocks[0].setH(listaStocks[0].getH()-altoMayor);
            altoMayor=0;
            anchoMayor=0;
            j++;
        }
        cromosomaTemp = convertirMatrizACromosoma(matriz);
        generarCromosoma(cromosoma,cromosomaTemp,listaPiezas2);
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
    
    // Ignoramos los valores dummy
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
    
    // Imprimir los tamaños calculados para ver si son correctos
    cout << "Tamaño numérico del padre1: " << tamanoPadre1 << endl;
    cout << "Tamaño numérico del padre2: " << tamanoPadre2 << endl;
    cout << "Tamaño numérico usado para el cruce: " << tamanoCruce << endl;
    
    Cromosoma hijo1 = padre1;  // Copias de los padres
    Cromosoma hijo2 = padre2;
    
    if (tamanoCruce == 0) {
        cout << "No hay parte numérica para realizar el cruce. Se devuelven los padres sin cambios." << endl;
        return make_pair(hijo1, hijo2);
    }

    // Seleccionar dos puntos de cruce aleatorios
    int punto1 = rand() % tamanoCruce;
    int punto2 = rand() % tamanoCruce;
    if (punto1 > punto2) swap(punto1, punto2);
    
    cout << "Punto de cruce 1: " << punto1 << ", Punto de cruce 2: " << punto2 << endl;
    
    // Realizar el cruce PMX solo en la porción numérica común
    cout << "Genes del padre1 antes del cruce: ";
    for (int i = 0; i < tamanoPadre1; ++i) {
        cout << padre1.getGene(i).getID() << " ";
    }
    cout << endl;

    cout << "Genes del padre2 antes del cruce: ";
    for (int i = 0; i < tamanoPadre2; ++i) {
        cout << padre2.getGene(i).getID() << " ";
    }
    cout << endl;
	
    // Mapeo parcial de genes entre los dos puntos de cruce
    for (int i = punto1; i <= punto2; ++i) {
        // Intercambiar genes entre los hijos en el rango seleccionado
        Pieza genPadre1 = padre1.getGene(i);
        Pieza genPadre2 = padre2.getGene(i);

        hijo1.setGene(i, genPadre2);
        hijo2.setGene(i, genPadre1);
    }
    
    // Imprimir genes de los hijos después del cruce
    cout << "Genes del hijo1 después del cruce: ";
    for (int i = 0; i < hijo1.getGenes().size(); ++i) {
        cout << hijo1.getGene(i).getID() << " ";
    }
    cout << endl;

    cout << "Genes del hijo2 después del cruce: ";
    for (int i = 0; i < hijo2.getGenes().size(); ++i) {
        cout << hijo2.getGene(i).getID() << " ";
    }
    cout << endl;
    
    // Resolver duplicados en hijo1
    for (int i = 0; i < tamanoCruce; ++i) {
        if (i < punto1 || i > punto2) {
            Pieza genPadre = hijo1.getGene(i);

            // Verificar si el gen ya está presente en otras partes del hijo1 fuera del rango de cruce
            bool duplicadoAntes = find_if(hijo1.getGenes().begin(), hijo1.getGenes().begin() + punto1,
                                          [&genPadre](const Pieza& pieza) { return pieza.getID() == genPadre.getID() && genPadre.getID() != -1; }) != hijo1.getGenes().begin() + punto1;

            bool duplicadoDespues = find_if(hijo1.getGenes().begin() + punto2 + 1, hijo1.getGenes().end(),
                                            [&genPadre](const Pieza& pieza) { return pieza.getID() == genPadre.getID() && genPadre.getID() != -1; }) != hijo1.getGenes().end();

            if (duplicadoAntes || duplicadoDespues) {

                // Reemplazar el duplicado con un gen de padre2 que no esté en el hijo1
                bool reemplazoHecho = false;
                for (int j = 0; j < tamanoCruce; ++j) {
                    Pieza genReemplazo = padre2.getGene(j);
                    if (find_if(hijo1.getGenes().begin(), hijo1.getGenes().end(),
                                [&genReemplazo](const Pieza& pieza) { return pieza.getID() == genReemplazo.getID(); }) == hijo1.getGenes().end()) {
                        hijo1.setGene(i, genReemplazo);
                        reemplazoHecho = true;
                        break;
                    }
                }
            }
        }
    }

    // Resolver duplicados en hijo2
    for (int i = 0; i < tamanoCruce; ++i) {
        if (i < punto1 || i > punto2) {
            Pieza genPadre = hijo2.getGene(i);

            // Verificar si el gen ya está presente en otras partes del hijo2 fuera del rango de cruce
            bool duplicadoAntes = find_if(hijo2.getGenes().begin(), hijo2.getGenes().begin() + punto1,
                                          [&genPadre](const Pieza& pieza) { return pieza.getID() == genPadre.getID() && genPadre.getID() != -1; }) != hijo2.getGenes().begin() + punto1;

            bool duplicadoDespues = find_if(hijo2.getGenes().begin() + punto2 + 1, hijo2.getGenes().end(),
                                            [&genPadre](const Pieza& pieza) { return pieza.getID() == genPadre.getID() && genPadre.getID() != -1; }) != hijo2.getGenes().end();

            if (duplicadoAntes || duplicadoDespues) {

                // Reemplazar el duplicado con un gen de padre1 que no esté en el hijo2
                bool reemplazoHecho = false;
                for (int j = 0; j < tamanoCruce; ++j) {
                    Pieza genReemplazo = padre1.getGene(j);
                    if (find_if(hijo2.getGenes().begin(), hijo2.getGenes().end(),
                                [&genReemplazo](const Pieza& pieza) { return pieza.getID() == genReemplazo.getID(); }) == hijo2.getGenes().end()) {
                        hijo2.setGene(i, genReemplazo);
                        reemplazoHecho = true;
                        break;
                    }
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

// Función para verificar si un cromosoma es una solución válida
bool verificarCromosoma(const Cromosoma& cromosoma, const Stock& stock) {
    float anchoMaximo = stock.getAncho();
    float altoMaximo = stock.getAlto();
    float anchoFilaActual = 0;
    float alturaFilaActual = 0;
    float alturaTotal = 0;

    for (const Pieza& pieza : cromosoma.getGenes()) {
        if (pieza.getID() == -1) break; // Ignorar posiciones vacías

        // Verificar si la pieza cabe en la fila actual
        if (anchoFilaActual + pieza.getW() <= anchoMaximo) {
            // Colocar la pieza en la fila actual
            anchoFilaActual += pieza.getW();
            alturaFilaActual = max(alturaFilaActual, pieza.getH());
        } else {
            // Mover a la siguiente fila
            alturaTotal += alturaFilaActual;
            if (alturaTotal + pieza.getH() > altoMaximo) {
                // La pieza no encaja en el stock, la solución es inválida
                return false;
            } else {
                // Colocar la pieza en la nueva fila
                anchoFilaActual = pieza.getW();
                alturaFilaActual = pieza.getH();
            }
        }
    }

    // Si todas las piezas encajan, la solución es válida
    return true;
}

void algoritmoGA(vector<Pieza>& listaPiezas, vector<Stock>& listaStocks,int tamanoPoblacion,
        int generaciones, double tasaMutacion) {
    
    int validoH1, validoH2;
    Poblacion poblacion;
    poblacion = generarPoblacionInicial(listaPiezas,listaStocks,tamanoPoblacion);
    Cromosoma hijo1, hijo2,hijo1Original,hijo2Original ;
    //poblacion.imprimir();
    
    // Calculamos el fitnees de cada solución para elegir las mejores
    for (Cromosoma& cromosoma : poblacion.getCromosomas()) {
        calcularDesperdicio(cromosoma, listaStocks[0]);
        validoH1 = verificarCromosoma(cromosoma,listaStocks[0]);
        for (int i = 0; i < cromosoma.getGenes().size(); ++i) {
            cout << cromosoma.getGene(i).getID() << " ";
        }
        cout << "Valido: " << validoH1 << endl;
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
            
            // Ingresan soluciones validas
            Cromosoma padre1 = seleccionarRuleta(poblacion);
            Cromosoma padre2;
            do {
                padre2 = seleccionarRuleta(poblacion);
            } while (padre1 == padre2);
            
            calcularDesperdicio(padre1, listaStocks[0]);
            calcularDesperdicio(padre2, listaStocks[0]);

            do {
                pair<Cromosoma, Cromosoma> hijos = cruzar(padre1, padre2);

                // Verificamos una solucion valida
                hijo1 = hijos.first;
                hijo2 = hijos.second;

                validoH1 = verificarCromosoma(hijo1,listaStocks[0]);
                validoH2 = verificarCromosoma(hijo2,listaStocks[0]);
                
            } while (validoH1 ==0 | validoH2==0);
            cout << "Genes del hijo1 después del cruce: ";
            for (int i = 0; i < hijo1.getGenes().size(); ++i) {
                cout << hijo1.getGene(i).getID() << " ";
            }
            cout << endl;
            cout << "Genes del hijo2 después del cruce: ";
            for (int i = 0; i < hijo2.getGenes().size(); ++i) {
                cout << hijo2.getGene(i).getID() << " ";
            }
            cout << endl;
            cout << "Cruce valido " << endl;
            
            hijo1Original = hijo1;
            hijo2Original = hijo2;
            
            do{
                hijo1 = hijo1Original;
                hijo2 = hijo2Original;
                
                mutar(hijo1, tasaMutacion);
                mutar(hijo2, tasaMutacion);

                validoH1 = verificarCromosoma(hijo1,listaStocks[0]);
                validoH2 = verificarCromosoma(hijo2,listaStocks[0]);
                
            }while(validoH1==0 | validoH2==0);
            cout << "Genes del hijo1 después de la mutación: ";
            for (int i = 0; i < hijo1.getGenes().size(); ++i) {
                cout << hijo1.getGene(i).getID() << " ";
            }
            cout << endl;
            cout << "Genes del hijo2 después de la mutación: ";
            for (int i = 0; i < hijo2.getGenes().size(); ++i) {
                cout << hijo2.getGene(i).getID() << " ";
            }
            cout << endl;
            cout << "Mutación valido " << endl;
            
            calcularDesperdicio(hijo1, listaStocks[0]);
            calcularDesperdicio(hijo2, listaStocks[0]);
            
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
    int generaciones = 10;
    double tasaMutacion = 0.1;
    
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
    
    algoritmoGA(listaPiezas,listaStocks,tamanoPoblacion,generaciones,tasaMutacion);
    cout << "Ejecución del algoritmo GA finalizada." << endl;
   
    return 0;
}

