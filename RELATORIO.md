# Relatorio - Jogo Snake com OpenGL

## 1. Objetivo

O projeto implementa o jogo Snake em C, utilizando OpenGL e FreeGLUT para criar
a janela, desenhar o tabuleiro e processar entrada e atualizacoes periodicas.

## 2. Implementacao

O tabuleiro possui uma grade logica de 30 por 20 celulas. A cobra e armazenada
como um vetor de coordenadas inteiras; o indice zero representa a cabeca. Ela
inicia com tres segmentos e avanca exatamente uma celula por atualizacao.

A logica verifica a proxima posicao antes de mover. Sair da grade ou ocupar uma
celula do proprio corpo produz Game Over. Quando a proxima celula contem a
comida, a cauda e preservada, a pontuacao e incrementada e uma nova comida e
sorteada entre as celulas livres. O intervalo do temporizador diminui a cada
cinco pontos, aumentando a velocidade.

Para impedir inversoes ilegais, cada entrada e comparada com a direcao atual.
Depois que uma mudanca valida e aceita, novas mudancas sao bloqueadas ate o
proximo movimento.

## 3. Conceitos de OpenGL utilizados

- Projecao ortografica com `glOrtho`, adequada ao tabuleiro bidimensional.
- Quadrilateros (`GL_QUADS`) para fundo visual, bordas, cobra e comida.
- Cores RGB distintas para cada elemento.
- Double buffering para evitar cintilacao durante a animacao.
- Texto bitmap do GLUT para pontuacao e mensagens.
- `glutTimerFunc` para atualizar o estado sem bloquear o event loop.
- Viewport com proporcao preservada durante o redimensionamento.

## 4. Testes realizados

Os testes foram executados no Ubuntu 24.04 pelo WSL 2, com GCC 13 e FreeGLUT
3.4.0. Foram realizadas as seguintes verificacoes:

- `make all test`: compilou o jogo e os testes com avisos tratados como erros;
  todos os testes da logica passaram.
- Testes com AddressSanitizer e UndefinedBehaviorSanitizer: passaram sem erros
  de memoria ou comportamento indefinido.
- Analise estatica do GCC com `-fanalyzer`: terminou sem diagnosticos.
- Inicializacao com `xvfb-run`: o processo abriu no display virtual e permaneceu
  ativo por tres segundos, ate o encerramento controlado por `timeout`.

Os testes automatizados cobrem estado inicial, movimento automatico, regras de
direcao, crescimento, pontuacao, progressao de velocidade, colisao com parede,
colisao com o corpo, passagem pela celula liberada pela cauda, posicionamento da
comida, pausa e preenchimento completo do tabuleiro.

Nao foi realizada uma partida manual completa nem uma avaliacao visual em uma
janela real neste ambiente. Essas verificacoes continuam necessarias antes da
entrega.

## 5. Resultados e discussao

A separacao entre regras e renderizacao permite testar movimento, crescimento,
colisoes, direcoes e velocidade sem depender de uma janela grafica. A grade
inteira evita erros de alinhamento e simplifica as colisoes. O temporizador do
GLUT mantem a interface responsiva. A curva de velocidade diminui o intervalo a
cada cinco pontos e permanece positiva ate o preenchimento completo da grade.

## 6. Captura de tela

> Inserir aqui uma captura de tela real do jogo em execucao antes da entrega.

Legenda sugerida: **Figura 1 - Jogo Snake em execucao, com cobra, comida,
bordas e pontuacao visiveis.**
