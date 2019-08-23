// #include "cstdlib"
// #include "TiposClasses.h"
#include "Wavefront.h"

#define MAX 9999

int campo[170][130];

std::vector<PosXY> caminho;
std::vector<Estado> timeAdversario;
std::vector<Estado> timeNosso;
PosXY bola;
Objetivo obj;
bool ignora = false;

// std::ostream& operator<<(std::ostream& os, const std::vector<PosXY> &input) {
//     // Printa o caminho
// 	for (auto const& i: input) {
// 		os << "[" << i.x << ", " << i.y << "] -> ";
// 	}
// 	return os;
// }

// float calculaDistEuclidianaRoboObj (int x, int y) {
//     // Baseado no codigo do jogo Pac-Man na hora de decidir o sentido do fantasma em uma encruzilhada
//     return std::sqrt((x - obj.x) * (x - obj.x) + (y - obj.y) * (y - obj.y));
// }

void inicializaWaveFront(){
    for(int i = 0; i < 170; i++){
        for(int j = 0; j < 130; j++){
            if ((i < 10 && j < 30) || (i < 10 && j > 100) || (i > 160 && j < 30) || (i > 160 && j > 100)) {
                campo[i][j] = -1;
            }
            else {
                campo[i][j] = MAX;
            }
        }
    }
}

void paredeObjetivo(int objx, int objy, float angulo){
    float x1, x2, x3, x4, y1, y2, y3, y4, aux;
    float angRad = angulo * M_PI/180;
    float seno = sin(angRad);
    float cosseno = cos(angRad);
    float tangenteTheta;
    float tangenteOrto;
    bool possuiTan = false;

    if (angulo != 0 && angulo != 90 && angulo != 180 && angulo != 270) {
        tangenteTheta = tan(angRad);
        tangenteOrto = tan(angRad + (M_PI/2));
        possuiTan = true;
    }

    // A abertura eh entre os pontos 3 e 4
    // Baseia-se na ideia que 0 eh paralelo ao eixo X
    // Calcula a rotacao na origem e depois translada os pontos marcadores para o objetivo real

    x1 = x2 = -4;   //1                     3
    x3 = x4 = 7;    //
    y1 = y3 = 4;    //      obj
    y2 = y4 = -4;   //
                    //2                     4
    
    aux = x1 * cosseno - y1 * seno;
    y1 = y1 * cosseno + x1 * seno;
    x1 = aux;

    aux = x2 * cosseno - y2 * seno;
    y2 = y2 * cosseno + x2 * seno;
    x2 = aux;

    aux = x3 * cosseno - y3 * seno;
    y3 = y3 * cosseno + x3 * seno;
    x3 = aux;

    aux = x4 * cosseno - y4 * seno;
    y4 = y4 * cosseno + x4 * seno;
    x4 = aux;

    // std::cout << "1: " << x1 << ' ' << y1 << '\n';
    // std::cout << "2: " << x2 << ' ' << y2 << '\n';
    // std::cout << "3: " << x3 << ' ' << y3 << '\n';
    // std::cout << "4: " << x4 << ' ' << y4 << '\n';
    // std::cout << "tangente: " << tangenteTheta << ':' << tangenteOrto << '\n';

    // Desenha os segmentos de reta para fazer a caixa aberta a partir de uma funcao de reta (usa a tangente)
    if(possuiTan){
        if (angulo < 90 || angulo > 270){
            for(int i = 0; i <= abs(x3 - x1); i++){
                aux = tangenteTheta * (i) + y1;
                campo[(int) x1+i+objx][(int) aux+objy] = -1;
                campo[(int) x1+i+objx-1][(int) aux+objy] = -1;
            }
            for(int i = 0; i <= abs(x4 - x2); i++){
                aux = tangenteTheta * (i) + y2;
                campo[(int) x2+i+objx][(int) aux+objy] = -1;
                campo[(int) x2+i+objx+1][(int) aux+objy] = -1;
            }
        }
        if (angulo > 0 || angulo < 180){
            for(int i = 0; i <= abs(x2 - x1); i++){
                aux = tangenteOrto * (i) + y1;
                campo[(int) x1+i+objx][(int) aux+objy] = -1;
                campo[(int) x1+i+objx-1][(int) aux+objy] = -1;
            }
        }
        if (angulo > 90 && angulo < 270){
            for(int i = 0; i <= abs(x3 - x1); i++){
                aux = tangenteTheta * (i) + y1;
                campo[(int) x1-i+objx][(int) aux+objy] = -1;
            }
            for(int i = 0; i <= abs(x4 - x2); i++){
                aux = tangenteTheta * (i) + y2;
                campo[(int) x2-i+objx][(int) aux+objy] = -1;
            }
        }
        if (angulo > 180){
            for(int i = 0; i <= abs(x2 - x1); i++){
                aux = tangenteOrto * (i) + y1;
                campo[(int) x1-i+objx][(int) aux+objy] = -1;
            }
        }
    }

    // Para angulos que nao possuem tangente os segmentos que formam a caixa sao paralelos aos eixos
    else if (angulo == 0 || angulo == 180){
        for(int i = 0; i <= abs(x3 - x1); i++)
            campo[(int) x1+i+objx][(int) y1+objy] = -1;

        for(int i = 0; i <= abs(x4 - x2); i++)
            campo[(int) x2+i+objx][(int) y2+objy] = -1;
        
        for(int i = 0; i <= abs(y2 - y1); i++)
            campo[(int) x1+objx][(int) y1+i+objy] = -1;
    }
    else if (angulo == 90 || angulo == 270){
        for(int i = 0; i <= abs(x2 - x1); i++)
            campo[(int) x1+i+objx][(int) y1+objy] = -1;

        for(int i = 0; i <= abs(y4 - y2); i++)
            campo[(int) x2+objx][(int) y2+i+objy] = -1;
        
        for(int i = 0; i <= abs(y3 - y1); i++)
            campo[(int) x1+objx][(int) y1+i+objy] = -1;
    }
    return;
}

int vizinhoMinimo(int x, int y){
    int minimo = MAX;
    //          O O O
    // Calcula  O R O
    //          O O O
    if(x > 0 && x < 169){
        //          X X X
        // Calcula  O R O
        //          X X X
        if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && campo[x+1][y] != -2)
            minimo = campo[x+1][y];
        
        if (campo[x-1][y] < minimo && campo[x-1][y] != -1 && campo[x-1][y] != -2)
            minimo = campo[x-1][y];

        //          O O O
        // Calcula  X R X
        //          O O O
        if(y > 0 && y < 129){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
        //          X X X
        // Calcula  X R X
        //          O O O
        else if(y == 0){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
        }
        //          O O O
        // Calcula  X R X
        //          X X X
        else if(y == 129) {
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
    }
    //          X O O
    // Calcula  X R O
    //          X O O
    else if(x == 0){
        if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && campo[x+1][y] != -2)
            minimo = campo[x+1][y];
        
        if(y > 0 && y < 129){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
        }
        else if(y == 0){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
        }
        else if(y == 129) {
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
        }
    }

    //          O O X
    // Calcula  O R X
    //          O O X
    else if (x == 169) {
        if (campo[x-1][y] < minimo && campo[x+1][y] != -1 && campo[x-1][y] != -2)
            minimo = campo[x-1][y];

        if(y > 0 && y < 129){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
        else if(y == 0){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
        }
        else if(y == 129) {
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
    }
    
        
    return minimo;
}

void calculaWaveFront(int objx, int objy, int objangulo, bool ignoraAdversario, Estado estado[NUM_ROBOS_TIME * 2 + 1], int mapa[170][130]){
    bola.x = estado[IND_BOLA].x;
    bola.y = estado[IND_BOLA].y;
    obj.x = objx;
    obj.y = objy;
    obj.angulo = objangulo;
    ignora = ignoraAdversario;

    // Inicia o campo
    inicializaWaveFront();
    
    if (ignora){
        // Coloca os robos existentes no campo como obstaculos
        for(int robo = NUM_ROBOS_TIME + 1; robo < NUM_ROBOS_TIME * 2 + 1; robo++){
            for(int i = estado[robo].x - 3; i < estado[robo].x + 3; i++){
                for(int j = estado[robo].y - 3; j < estado[robo].y + 3; j++){
                    campo[i][j] = -1;
                }
            }
            campo[(int) estado[robo - NUM_ROBOS_TIME].x][(int) estado[robo - NUM_ROBOS_TIME].y] = -1;
        }
    }

    // Calculoa paredes ao redor do objetivo para o robo chegar em determinado angulo
    paredeObjetivo((int) obj.x, (int) obj.y, obj.angulo);
    campo[(int) obj.x][(int) obj.y] = 0;
    
    // Preenche o campo com os valores
    bool convergiu = false;
    int valorMinimo = 0;
    int x = 0;
    int y = 0;
    while (campo[160][129] == MAX || campo[10][0] == MAX){
        x = 0;
        y = 0;
        convergiu = false;
        do {
            if(campo[x][y] != -1 && campo[x][y] != -2){

                valorMinimo = vizinhoMinimo(x, y);
                
                if (valorMinimo != MAX && campo[x][y] > valorMinimo){
                    campo[x][y] = valorMinimo + 1;
                    // std::cout << x <<'\n';
                }
            }

            x++;
            if(x == 169 && y != 129){
                y++;
                x = 0;
            }
            else if (x == 169 && y == 129){
                convergiu = true;
                // std::cout << "Terminou o WaveFront2" << '\n';
            }
        } while (!convergiu);
    }
}

// void calculaCaminho() {
//     int x, y, minimo, distMin;
//     bool convergiu = false;
//     PosXY aux, auxPassado;

//     x = state.teamYellow[0].x;
//     y = state.teamYellow[0].y;
//     minimo = MAX;

//     caminho.clear();

//     // Faz o caminho de uma maneira gulosa
//     // Se existe duas casas com o mesmo valor onde o robo pode prosseguir, usa-se uma estrategia baseada
//     // nos inimigos do Pac-Man (os fantasminhas):
//         // Quando um fantasminha chegava em uma encruzilhada ele decidia o sentido fazendo um
//         // calculo de distancia euclidiana entre as posicoes previstas e seu objetivo, escolhendo entao
//         // a posicao com a menor distancia euclidiana
//     int contador = 0;
//     do {
//         contador++;
//         minimo = MAX;
//         distMin = MAX;
        
//         auxPassado = aux;
//         // std::cout << x << " " << y << " " << campo[x+1][y-1] << '\n';

//         //          O O O
//         // Calcula  O R O
//         //          O O O
//         if(x > 0 && x < 169){
//             //          X X X
//             // Calcula  O R O
//             //          X X X
//             if (campo[x-1][y] < minimo && campo[x-1][y] != -1 && calculaDistEuclidianaRoboObj(x-1, y) < distMin){
//                 minimo = campo[x-1][y];
//                 aux.x = x-1;
//                 aux.y = y;
//                 distMin = calculaDistEuclidianaRoboObj(x-1, y);
//             }

//             if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && calculaDistEuclidianaRoboObj(x+1, y) < distMin){
//                 minimo = campo[x+1][y];
//                 aux.x = x+1;
//                 aux.y = y;
//                 distMin = calculaDistEuclidianaRoboObj(x+1, y);
//             }

//             //          O O O
//             // Calcula  X R X
//             //          O O O
//             if(y > 0 && y < 129){
//                 if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
//                     minimo = campo[x][y+1];
//                     aux.x = x;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y+1);
//                 }
//                 if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
//                     minimo = campo[x][y-1];
//                     aux.x = x;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y-1);
//                 }
//                 if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
//                     minimo = campo[x+1][y+1];
//                     aux.x = x+1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
//                 }
//                 if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
//                     minimo = campo[x+1][y-1];
//                     aux.x = x+1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
//                 }
//                 if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
//                     minimo = campo[x-1][y+1];
//                     aux.x = x-1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
//                 }
//                 if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
//                     minimo = campo[x-1][y-1];
//                     aux.x = x-1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
//                 }
//             }
//             //          X X X
//             // Calcula  X R X
//             //          O O O
//             else if(y == 0){
//                 if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
//                     minimo = campo[x][y+1];
//                     aux.x = x;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y+1);
//                 }
//                 if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
//                     minimo = campo[x+1][y+1];
//                     aux.x = x+1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
//                 }
//                 if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
//                     minimo = campo[x-1][y+1];
//                     aux.x = x-1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
//                 }
//             }
//             //          O O O
//             // Calcula  X R X
//             //          X X X
//             else if(y == 129) {
//                 if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
//                     minimo = campo[x][y-1];
//                     aux.x = x;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y-1);
//                 }
//                 if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
//                     minimo = campo[x+1][y-1];
//                     aux.x = x+1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
//                 }
//                 if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
//                     minimo = campo[x-1][y-1];
//                     aux.x = x-1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
//                 }
//             }
//         }

//         //          X O O
//         // Calcula  X R O
//         //          X O O
//         else if(x == 0){
//             if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && calculaDistEuclidianaRoboObj(x+1, y) < distMin){
//                 minimo = campo[x+1][y];
//                 aux.x = x+1;
//                 aux.y = y;
//                 distMin = calculaDistEuclidianaRoboObj(x+1, y);
//             }
            
//             if(y > 0 && y < 129){
//                 if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
//                     minimo = campo[x][y+1];
//                     aux.x = x;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y+1);
//                 }
//                 if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
//                     minimo = campo[x][y-1];
//                     aux.x = x;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y-1);
//                 }
//                 if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
//                     minimo = campo[x+1][y+1];
//                     aux.x = x+1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
//                 }
//                 if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
//                     minimo = campo[x+1][y-1];
//                     aux.x = x+1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
//                 }
//             }
//             else if(y == 0){
//                 if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
//                     minimo = campo[x][y+1];
//                     aux.x = x;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y+1);
//                 }
//                 if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
//                     minimo = campo[x+1][y+1];
//                     aux.x = x+1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
//                 }
//             }
//             else if(y == 129) {
//                 if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
//                     minimo = campo[x][y-1];
//                     aux.x = x;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y-1);
//                 }
//                 if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
//                     minimo = campo[x+1][y-1];
//                     aux.x = x+1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
//                 }
//             }
//         }

//         //          O O X
//         // Calcula  O R X
//         //          O O X
//         else if (x == 169) {
//             if (campo[x-1][y] < minimo && campo[x+1][y] != -1 && calculaDistEuclidianaRoboObj(x-1, y) < distMin){
//                 minimo = campo[x-1][y];
//                 aux.x = x-1;
//                 aux.y = y;
//                 distMin = calculaDistEuclidianaRoboObj(x-1, y);
//             }

//             if(y > 0 && y < 129){
//                 if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
//                     minimo = campo[x][y+1];
//                     aux.x = x;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y+1);
//                 }
//                 if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
//                     minimo = campo[x][y-1];
//                     aux.x = x;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y-1);
//                 }
//                 if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
//                     minimo = campo[x-1][y+1];
//                     aux.x = x-1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
//                 }
//                 if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
//                     minimo = campo[x-1][y-1];
//                     aux.x = x-1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
//                 }
//             }
//             else if(y == 0){
//                 if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
//                     minimo = campo[x][y+1];
//                     aux.x = x;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y+1);
//                 }
//                 if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
//                     minimo = campo[x-1][y+1];
//                     aux.x = x-1;
//                     aux.y = y+1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
//                 }
//             }
//             else if(y == 129) {
//                 if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
//                     minimo = campo[x][y-1];
//                     aux.x = x;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x, y-1);
//                 }
//                 if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
//                     minimo = campo[x-1][y-1];
//                     aux.x = x-1;
//                     aux.y = y-1;
//                     distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
//                 }
//             }
//         }
//         // std::cout << minimo << "\n";

//         if (auxPassado != aux){
//             caminho.push_back(aux);
//             x = aux.x;
//             y = aux.y;
//         }
        
//         if (minimo == 0){
//             convergiu = true;
//         }
//         if(contador == 500)
//             break;
        
//     } while (!convergiu && contador < 200);
// }