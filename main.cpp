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

using namespace std;

int numPiezas = 10;
vector<vector<int>> matriz(numPiezas, vector<int>(numPiezas, -1));
vector<vector<int>> solucion(numPiezas, vector<int>(numPiezas, -1));
int anchoMayor,altoMayor;
double desTotal;

vector<vector<int>> generarPoblacionInicial(vector<Pieza>& listaPiezas2, vector<Stock>& listaStocks2, int tamanoPoblacion) {
    vector<vector<int>> poblacion;
    
    Stock solucion;
    int numPiezaLista=0,indiceAleatorio=0,entra=0,menorH=100,a,b;
    int piezaEscogida,i=0,j=0,piezaPasada,ancho,alto,intentos=0,indice;
    double feromonas,heuristica,probabillidad=100,mProb=0;
    vector<int> indicePiezaEscogida;
    
    vector<Pieza> listaPiezas = listaPiezas2;
    vector<Stock> listaStocks = listaStocks2;
    
    for (const Pieza& pieza2 : listaPiezas) {
        if (listaPiezas.empty()) break;
        entra=0;
        while(entra==0 && intentos < (numPiezas-1)){
            indiceAleatorio = rand() % (numPiezas-1); // Random por numero de piezas
            // Obtenemos el indice del id que queremos
            auto it = find_if(listaPiezas.begin(), listaPiezas.end(), 
                          [indiceAleatorio](const Pieza& pieza) {
                              return pieza.getID() == indiceAleatorio;
                          });
                          
            if (it != listaPiezas.end()) {
                indice = distance(listaPiezas.begin(), it);
                listaPiezas[indice].imprimirPieza();
                cout << "Ancho x Alto:" << anchoMayor << " , " << altoMayor<<endl;
            }           
            
            if (find(indicePiezaEscogida.begin(), indicePiezaEscogida.end(), indiceAleatorio) == indicePiezaEscogida.end()) {
                if (listaPiezas[indice].getW() <= listaStocks[0].getW() && listaPiezas[indice].getH() <= listaStocks[0].getH()) {
                    entra=1;
                    indicePiezaEscogida.push_back(indiceAleatorio);
                }else{
                    entra=0;
                }
            }
            intentos++;              
                          
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
        listaStocks[0].setH(listaStocks[0].getH()-altoMayor);
        j++;
    }
    
    return poblacion;
}

void imprimirMatriz(const vector<vector<int>>& matriz) {
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            cout << matriz[i][j] << " ";  // Imprime cada elemento con espacio
        }
        cout << endl;  // Salto de línea al final de cada fila
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

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));
    int numPiezas = 40; 
    int tamanoPoblacion = 10; 
    int generaciones = 50;
    double tasaMutacion = 0.1;
    
    vector<vector<int>> poblacion;
    vector<Pieza> listaPiezas = generarListaPiezas(numPiezas);
    vector<Stock> listaStocks = generarListaStocks(1);
    

    sort(listaStocks.begin(), listaStocks.end(), compararStocks);
    sort(listaPiezas.begin(), listaPiezas.end(), compararPiezas);

    //AlgoritmoACO(nHormigas, maxIter, alpha, beta, rho, tol, listaPiezas, listaStocks, grafo);
    poblacion = generarPoblacionInicial(listaPiezas,listaStocks, tamanoPoblacion);
    cout << "Ejecución del algoritmo GA finalizada." << endl;
    
    cout << "Lista de Piezas:\n";
    for (const Pieza& pieza : listaPiezas) {
        pieza.imprimirPieza();
    }
    cout << "\nLista de Stock:\n";
    listaStocks[0].imprimirStock();
    
   cout << endl;
   imprimirMatriz(matriz);
    
    return 0;
}

