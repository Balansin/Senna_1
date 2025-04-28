#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_POINTS = 100;
const float Y_MIN = -2.0f;
const float Y_MAX =  2.0f;
const char* CSV_PATH = "/home/safena/Senna_1/Log/dados_mqtt.csv";

// Lê os dados do CSV
void ler_csv(std::vector<float>& x_vals, std::vector<float>& y_vals, std::vector<float>& z_vals) {
    std::ifstream file(CSV_PATH);
    std::string line;
    x_vals.clear(); y_vals.clear(); z_vals.clear();

    std::getline(file, line); // Cabeçalho
    std::vector<std::string> linhas;

    while (std::getline(file, line)) {
        linhas.push_back(line);
    }

    int start = std::max(0, (int)linhas.size() - MAX_POINTS);
    for (int i = start; i < linhas.size(); ++i) {
        std::istringstream ss(linhas[i]);
        std::string token;
        std::getline(ss, token, ','); // tempo
        std::getline(ss, token, ','); x_vals.push_back(std::stof(token));
        std::getline(ss, token, ','); y_vals.push_back(std::stof(token));
        std::getline(ss, token, ','); z_vals.push_back(std::stof(token));
    }
}

// Converte valor para coordenada Y na tela
int valor_para_y(float valor) {
    float proporcao = (valor - Y_MIN) / (Y_MAX - Y_MIN);
    return HEIGHT - static_cast<int>(proporcao * HEIGHT);
}

// Desenha grade e eixos
void desenhar_grid(SDL_Renderer* ren) {
    SDL_SetRenderDrawColor(ren, 220, 220, 220, 255);
    for (int i = 1; i < 5; ++i) {
        int y = i * HEIGHT / 5;
        SDL_RenderDrawLine(ren, 0, y, WIDTH, y);
    }
    SDL_RenderDrawLine(ren, 0, HEIGHT / 2, WIDTH, HEIGHT / 2); // linha central

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    // Eixo Y indicador (esquerda)
    SDL_RenderDrawLine(ren, 50, 0, 50, HEIGHT);
    // Eixo X indicador (rodapé)
    SDL_RenderDrawLine(ren, 0, HEIGHT - 30, WIDTH, HEIGHT - 30);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("Acelerômetro em Tempo Real", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event e;
    std::vector<float> x_vals, y_vals, z_vals;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        ler_csv(x_vals, y_vals, z_vals);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); // fundo branco
        SDL_RenderClear(ren);

        desenhar_grid(ren);

        auto plot_line = [&](const std::vector<float>& vals, SDL_Color color) {
            SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, 255);
            for (int i = 1; i < vals.size(); ++i) {
                int x1 = 50 + (i - 1) * (WIDTH - 50) / MAX_POINTS;
                int x2 = 50 + i * (WIDTH - 50) / MAX_POINTS;
                int y1 = valor_para_y(vals[i - 1]);
                int y2 = valor_para_y(vals[i]);
                SDL_RenderDrawLine(ren, x1, y1, x2, y2);
            }
        };

        plot_line(x_vals, {255, 0, 0});   // vermelho para eixo X
        plot_line(y_vals, {0, 200, 0});   // verde para eixo Y
        plot_line(z_vals, {0, 0, 255});   // azul para eixo Z

        SDL_RenderPresent(ren);
        SDL_Delay(100);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
