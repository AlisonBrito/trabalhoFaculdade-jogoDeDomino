#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

//representa uma peca de dominó, com os valores dos dois lados
struct peca
{
    int lado1;
    int lado2;
};

//guarda as peças que ainda podem ser compradas durante a partida
struct monte
{
    peca *pecas;
    int qtd;
};

//cada no armazena uma peça e aponta para o no anterior e o próximo
struct no
{
    peca valor;
    no *anterior;
    no *proximo;
};

//a mesa é uma lista duplamente encadeada: início e fim permitem inserir nas duas pontas
struct mesa
{
    no *inicio;
    no *fim;
    int quantidade;
};

//cria as 28 combinações únicas do dominó tradicional
peca *gerarPecas()
{
    peca *pecas = new peca[28];

    int contador = 0;

    for (int i = 0; i <= 6; i++)
    {
        for (int j = i; j <= 6; j++)
        {
            pecas[contador].lado1 = i;
            pecas[contador].lado2 = j;

            contador++;
        }
    }
    return pecas;
}

//embaralha as peças trocando cada posição por uma posição aleatória anterior ou igual
void embaralharPecas(peca *pecas, int tam)
{
    for (int i = tam - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        peca aux = pecas[i];
        pecas[i] = pecas[j];
        pecas[j] = aux;
    }
}

//entrega sete peças para cada jogador, usando o vetor de peças já embaralhadas
void distribuirPecas(peca *pecas, int qtdJogadores, peca **jogadores)
{
    int indice = 0;

    for (int i = 0; i < qtdJogadores; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            jogadores[i][j] = pecas[indice];
            indice++;
        }
    }
}

//retira a última peça disponível do monte e reduz a quantidade restante
peca comprarPeca(monte &m)
{
    if (m.qtd == 0)
    {
        cout << "O monte esta vazio!\n";
        return {-1, -1};
    }

    peca comprada = m.pecas[m.qtd - 1];

    m.qtd--;

    return comprada;
}

//remove uma peca da mão deslocando as outras uma posição pra esquerda
//a quantidade é passada por referência para atualizar o total de peças do jogador
void removerPeca(peca *mao, int &quantidade, int indiceEscolhido)
{
    if (indiceEscolhido < 0 || indiceEscolhido >= quantidade)
    {
        cout << "Indice invalido!\n";
        return;
    }

    for (int i = indiceEscolhido; i < quantidade - 1; i++)
    {
        mao[i] = mao[i + 1];
    }

    quantidade--;
}

//inicializa a lista vazia: nao há primeiro nem ultimo nó
void inicializarMesa(mesa &m)
{
    m.inicio = nullptr;
    m.fim = nullptr;
    m.quantidade = 0;
}

//tenta encaixar uma peca na extremidade direita da mesa e se necessário inverte os lados (retorna false se não houver encaixe)
bool inserirDireita(mesa &m, peca novaPeca)
{
    if (m.inicio != nullptr)
    {
        int extremidade = m.fim->valor.lado2;

        if (novaPeca.lado2 == extremidade)
        {
            int aux = novaPeca.lado1;
            novaPeca.lado1 = novaPeca.lado2;
            novaPeca.lado2 = aux;
        }
        else
        {
            return false;
        }
    }

    no *novo = new no;

    novo->valor = novaPeca;
    novo->anterior = m.fim;
    novo->proximo = nullptr;

    if (m.inicio == nullptr)
    {
        m.inicio = novo;
    }
    else
    {
        m.fim->proximo = novo;
    }

    m.fim = novo;
    m.quantidade++;

    return true;
}

//tenta encaixar uma peca na extremidade esquerda da mesa
//atualiza os ponteiros da lista e inverte a peça quando necessário
bool inserirEsquerda(mesa &m, peca novaPeca)
{
    if (m.inicio != nullptr)
    {
        int extremidade = m.inicio->valor.lado1;

        if (novaPeca.lado1 == extremidade)
        {
            int aux = novaPeca.lado1;
            novaPeca.lado1 = novaPeca.lado2;
            novaPeca.lado2 = aux;
        }
        else
        {
            return false;
        }
    }

    no *novo = new no;

    novo->valor = novaPeca;
    novo->anterior = nullptr;
    novo->proximo = m.inicio;

    if (m.fim == nullptr)
    {
        m.fim = novo;
    }
    else
    {
        m.inicio->anterior = novo;
    }

    m.inicio = novo;
    m.quantidade++;

    return true;
}

//percorre a lista e libera cada nó alocado dinamicamente
void liberarMesa(mesa &m)
{
    no *atual = m.inicio;

    while (atual != nullptr)
    {
        no *proximo = atual->proximo;
        delete atual;
        atual = proximo;
    }

    m.inicio = nullptr;
    m.fim = nullptr;
    m.quantidade = 0;
}

//percorre a lista do início ao fim e mostra as peças que estão na mesa
void exibirMesa(mesa &m)
{
    no *atual = m.inicio;

    cout << "\nMesa:\n";

    while (atual != nullptr)
    {
        cout << atual->valor.lado1
             << " | "
             << atual->valor.lado2;

        if (atual->proximo != nullptr)
        {
            cout << " -- ";
        }

        atual = atual->proximo;
    }

    cout << "\n";
}

void exibirMao(peca *mao, int quantidade)
{
    for (int i = 0; i < quantidade; i++)
    {
        cout << "[" << mao[i].lado1 << " | " << mao[i].lado2 << "] ";
    }
    cout << "\n";
}

//mostra a mão numerando as peças para que o jogador escolha pelo índice dela
void exibirMaoJogador(peca *mao, int quantidade)
{
    for (int i = 0; i < quantidade; i++)
    {
        cout << i << " - [" << mao[i].lado1 << " | " << mao[i].lado2 << "]\t";
    }
}

//valida a escolha, tenta inserir a peca na extremidade escolhida e por fim remove da mão
//retorna true se a jogada for realizada e false se houver algum erro ou falta de encaixe
bool jogarPeca(mesa &tabuleiro, peca *mao, int &quantidade, int indiceEscolhido, int lado)
{
    if (indiceEscolhido < 0 || indiceEscolhido >= quantidade)
    {
        cout << "Indice invalido!\n";
        return false;
    }

    if (lado != 0 && lado != 1)
    {
        cout << "Lado invalido! Use 0 para esquerda ou 1 para direita.\n";
        return false;
    }

    peca escolhida = mao[indiceEscolhido];
    bool inseriu;

    if (lado == 0)
    {
        inseriu = inserirEsquerda(tabuleiro, escolhida);
    }
    else
    {
        inseriu = inserirDireita(tabuleiro, escolhida);
    }

    if (!inseriu)
    {
        cout << "A peca nao encaixa nessa extremidade!\n";
        return false;
    }

    removerPeca(mao, quantidade, indiceEscolhido);
    return true;
}

//verifica se alguma peça da mão combina com uma das duas extremidades da mesa
bool temJogada(mesa &tabuleiro, peca *mao, int quantidade)
{
    if (tabuleiro.inicio == nullptr)
    {
        return quantidade > 0;
    }

    int esquerda = tabuleiro.inicio->valor.lado1;
    int direita = tabuleiro.fim->valor.lado2;

    for (int i = 0; i < quantidade; i++)
    {
        if (mao[i].lado1 == esquerda || mao[i].lado2 == esquerda || mao[i].lado1 == direita || mao[i].lado2 == direita)
        {
            return true;
        }
    }

    return false;
}

//soma os dois lados de todas as peças restantes na mão do jogador
int calcularPontos(peca *mao, int quantidade)
{
    int pontos = 0;

    for (int i = 0; i < quantidade; i++)
    {
        pontos += mao[i].lado1 + mao[i].lado2;
    }

    return pontos;
}

int main()
{
    srand(time(NULL));

    int qtdJogadores;

    cout << "Quantidade de jogadores (2 a 4): ";
    cin >> qtdJogadores;

    if (qtdJogadores < 2 || qtdJogadores > 4)
    {
        cout << "Quantidade invalida!\n";
        return 1;
    }

    peca *pecas = gerarPecas();
    embaralharPecas(pecas, 28);

    //aloca dinamicamente um vetor de mãos (uma para cada jogador)
    peca **jogadores = new peca *[qtdJogadores];
    int *qtdPecasJogador = new int[qtdJogadores];

    for (int i = 0; i < qtdJogadores; i++)
    {
        jogadores[i] = new peca[28];
        qtdPecasJogador[i] = 7;
    }

    // distribui sete peças para cada jogador e as restantes vão pro monte
    distribuirPecas(pecas, qtdJogadores, jogadores);

    int indice = qtdJogadores * 7;

    monte m;
    m.qtd = 28 - indice;
    m.pecas = nullptr;

    if (m.qtd > 0)
    {
        m.pecas = new peca[m.qtd];

        for (int i = 0; i < m.qtd; i++)
        {
            m.pecas[i] = pecas[indice + i];
        }
    }

    //prepara a mesa e as variáveis que controlam os turnos e o fim da partida
    mesa tabuleiro;
    inicializarMesa(tabuleiro);

    int jogadorAtual = 0;
    int passagensConsecutivas = 0;
    bool jogoAtivo = true;

    cout << "\n=== INICIO DO JOGO ===\n";

    //o jogo continua enquanto não houver vencedor ou bloqueio
    while (jogoAtivo)
    {
        cout << "\n--------------------------------\n";
        cout << "Vez do Jogador " << jogadorAtual + 1 << "\n";

        exibirMesa(tabuleiro);

        cout << "\nSua mao:\n";
        exibirMaoJogador(jogadores[jogadorAtual], qtdPecasJogador[jogadorAtual]);

        //se nao houver jogada, o jogador compra peças até conseguir jogar ou o monte acabar
        while (!temJogada(tabuleiro, jogadores[jogadorAtual], qtdPecasJogador[jogadorAtual]) && m.qtd > 0)
        {
            cout << "Voce nao tem jogada possivel. Comprando peca...\n";

            peca comprada = comprarPeca(m);

            jogadores[jogadorAtual][qtdPecasJogador[jogadorAtual]] = comprada;

            qtdPecasJogador[jogadorAtual]++;

            cout << "Peca comprada: " << comprada.lado1 << " | " << comprada.lado2 << "\n";

            cout << "\nSua mao atualizada:\n";
            exibirMaoJogador(jogadores[jogadorAtual], qtdPecasJogador[jogadorAtual]);
        }

        //sem jogada e sem peças no monte, o jogador passa a vez
        if (!temJogada(tabuleiro, jogadores[jogadorAtual], qtdPecasJogador[jogadorAtual]))
        {
            cout << "Jogador " << jogadorAtual + 1 << " passou a vez.\n";
            passagensConsecutivas++;
        }
        else
        {
            int indiceEscolhido;
            int lado;
            bool jogadaRealizada = false;

            while (!jogadaRealizada)
            {
                cout << "\nEscolha o indice da peca: ";
                cin >> indiceEscolhido;

                cout << "Esquerda (0) ou direita (1)? ";
                cin >> lado;

                jogadaRealizada = jogarPeca(tabuleiro, jogadores[jogadorAtual], qtdPecasJogador[jogadorAtual], indiceEscolhido, lado);
            }

            cout << "Jogada realizada!\n";

            exibirMesa(tabuleiro);

            passagensConsecutivas = 0;

            if (qtdPecasJogador[jogadorAtual] == 0)
            {
                cout << "\nJogador " << jogadorAtual + 1 << " venceu! Ficou sem pecas!\n";
                jogoAtivo = false;
            }
        }

        //se todos os jogadores passarem em sequência, a partida esta bloqueada
        if (jogoAtivo && passagensConsecutivas >= qtdJogadores)
        {
            cout << "\nA partida bloqueou!\n";
            cout << "Contando os pontos restantes...\n";

            int menorPontuacao = calcularPontos(jogadores[0], qtdPecasJogador[0]);

            for (int i = 1; i < qtdJogadores; i++)
            {
                int pontos = calcularPontos(jogadores[i], qtdPecasJogador[i]);

                if (pontos < menorPontuacao)
                {
                    menorPontuacao = pontos;
                }
            }

            int vencedor = -1;
            int quantidadeVencedores = 0;

            for (int i = 0; i < qtdJogadores; i++)
            {
                int pontos = calcularPontos(jogadores[i], qtdPecasJogador[i]);

                cout << "Jogador " << i + 1 << ": " << pontos << " pontos\n";

                if (pontos == menorPontuacao)
                {
                    vencedor = i;
                    quantidadeVencedores++;
                }
            }

            if (quantidadeVencedores > 1)
            {
                cout << "A partida terminou empatada!\n";
            }
            else
            {
                cout << "Jogador " << vencedor + 1 << " venceu com " << menorPontuacao << " pontos!\n";
            }
            jogoAtivo = false;
        }

        if (jogoAtivo)
        {
            jogadorAtual = (jogadorAtual + 1) % qtdJogadores;
        }
    }
    //libera toda a memória alocada dinamicamente antes de encerrar
    liberarMesa(tabuleiro);
    delete[] m.pecas;

    for (int i = 0; i < qtdJogadores; i++)
    {
        delete[] jogadores[i];
    }

    delete[] jogadores;
    delete[] qtdPecasJogador;
    delete[] pecas;

    cout << "\n=== FIM DO JOGO ===\n";

    return 0;
}
