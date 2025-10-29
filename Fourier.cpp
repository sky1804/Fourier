#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>

#define M_PI 3.14159265358979323846

using namespace std;

// ==========================
// Funções base
// ==========================
double f_sawtooth(double x) {
    return x; // definida em [-π, π]
}

double f_square(double x) {
    return (x >= 0) ? 1.0 : -1.0;
}

// ==========================
// Coeficientes da Série de Fourier
// ==========================
double a0(double (*f)(double)) {
    double soma = 0.0;
    int N = 1000;
    double dx = (2 * M_PI) / N;
    for (int i = 0; i < N; ++i) {
        double x = -M_PI + i * dx;
        soma += f(x);
    }
    return (1.0 / M_PI) * soma * dx;
}

double an(double (*f)(double), int n) {
    double soma = 0.0;
    int N = 1000;
    double dx = (2 * M_PI) / N;
    for (int i = 0; i < N; ++i) {
        double x = -M_PI + i * dx;
        soma += f(x) * cos(n * x);
    }
    return (1.0 / M_PI) * soma * dx;
}

double bn(double (*f)(double), int n) {
    double soma = 0.0;
    int N = 1000;
    double dx = (2 * M_PI) / N;
    for (int i = 0; i < N; ++i) {
        double x = -M_PI + i * dx;
        soma += f(x) * sin(n * x);
    }
    return (1.0 / M_PI) * soma * dx;
}

// ==========================
// Soma Parcial da Série Sn(x)
// ==========================
double Sn(double (*f)(double), double x, int N) {
    double soma = a0(f) / 2.0;
    for (int n = 1; n <= N; ++n) {
        soma += an(f, n) * cos(n * x) + bn(f, n) * sin(n * x);
    }
    return soma;
}

// ==========================
// Função principal
// ==========================
int main() {
    cout << "==============================" << endl;
    cout << "   SERIE DE FOURIER - DEMO   " << endl;
    cout << "==============================" << endl;

    cout << "Escolha a funcao:\n";
    cout << "1 - Dente de serra (f(x) = x)\n";
    cout << "2 - Onda quadrada (f(x) = -1, se -pi < x <= 0\n ________________________ 1, se 0 < x <= pi)\n";
    cout << "Opcao: ";

    int opcao;
    cin >> opcao;

    double (*f)(double);
    string nome_func;

    if (opcao == 1) {
        f = f_sawtooth;
        nome_func = "dente_de_serra";
        cout << "\n[INFO] Funcao escolhida: f(x) = x (Dente de Serra)" << endl;
    }
    else {
        f = f_square;
        nome_func = "onda_quadrada";
        cout << "\n[INFO] Funcao escolhida: Onda Quadrada" << endl;
    }

    cout << "\n[ETAPA 1] Calculando coeficientes de Fourier..." << endl;
    double a0_val = a0(f);
    cout << "   a0 = " << a0_val << endl;

    // Pré-calcular an e bn até n=30
    vector<double> an_vals(31), bn_vals(31);
    for (int n = 1; n <= 30; ++n) {
        an_vals[n] = an(f, n);
        bn_vals[n] = bn(f, n);
        cout << "   n=" << n << " -> an=" << an_vals[n] << ", bn=" << bn_vals[n] << endl;
    }

    cout << "\n[ETAPA 2] Gerando dados para o grafico..." << endl;
    ofstream arquivo("dados_" + nome_func + ".dat");
    double dx = 0.01;

    for (double x = -M_PI; x <= M_PI; x += dx) {
        arquivo << x << " " << f(x);
        // adicionar S_n(x) para diferentes valores de n
        vector<int> N_vals = { 1, 5, 10, 15, 20, 25, 30 };
        for (int N : N_vals) {
            double soma = a0_val / 2.0;
            for (int n = 1; n <= N; ++n)
                soma += an_vals[n] * cos(n * x) + bn_vals[n] * sin(n * x);
            arquivo << " " << soma;
        }
        arquivo << "\n";
    }

    arquivo.close();
    cout << "   -> Dados gravados em: dados_" << nome_func << ".dat\n";

    cout << "\n[ETAPA 3] Gerando script do gnuplot..." << endl;
    ofstream gp("plot_" + nome_func + ".plt");
    gp << "set title 'Serie de Fourier - " << nome_func << "'\n";
    gp << "set xlabel 'x'\n";
    gp << "set ylabel 'f(x) e S_n(x)'\n";
    gp << "plot 'dados_" << nome_func << ".dat' using 1:2 with lines title 'f(x)' lw 2";
    vector<int> N_vals = { 1, 5, 10, 15, 20, 25, 30 };
    int cor = 1;
    for (int N : N_vals) {
        gp << ", '' using 1:" << (2 + cor) << " with lines title 'S_" << N << "(x)'";
        cor++;
    }
    gp << "\npause -1\n";
    gp.close();

    cout << "   -> Script gerado: plot_" << nome_func << ".plt\n";

    cout << "\n[ETAPA 4] Exibindo grafico via gnuplot...\n";

    // Monta o comando
    string comando = "\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" plot_" + nome_func + ".plt";

    // Print do comando para debug
    cout << "   [DEBUG] Comando que sera executado: " << comando << endl;

    // Executa o comando
    int resultado = system(comando.c_str());

    // Verifica se funcionou
    if (resultado != 0) {
        cout << "[ERRO] Não foi possível executar o Gnuplot.\n";
        cout << "Verifique se o caminho do executável está correto e se o arquivo .plt existe.\n";
    }
    else {
        cout << "[SUCESSO] Gnuplot executado com sucesso!\n";
    }

    cout << "\n[FINALIZADO] Processo completo com sucesso!\n";
    cout << "Verifique o terminal e o grafico para observacoes sobre convergencia e Gibbs.\n";
    cout << "=============================================\n";

    return 0;
}