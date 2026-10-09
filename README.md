# Snake em C com OpenGL

Jogo Snake desenvolvido em C com OpenGL e FreeGLUT. A simulacao usa uma grade
logica de 30 por 20 celulas e o movimento e executado pelo temporizador do GLUT,
sem laco bloqueante ou `sleep`.

## Dependencias no Ubuntu/Linux

Instale o compilador, o Make e o FreeGLUT:

```bash
sudo apt update
sudo apt install build-essential freeglut3-dev
```

Em outra distribuicao, instale os pacotes equivalentes do GCC, Make, OpenGL e
FreeGLUT.

## Compilar e executar

Na pasta do projeto:

```bash
make
make run
```

O executavel sera criado em `build/snake`.

Para compilar e executar os testes da logica sem abrir uma janela:

```bash
make test
```

Para remover os arquivos gerados:

```bash
make clean
```

## Controles

- Setas ou `W`, `A`, `S`, `D`: mudar a direcao.
- `P`: pausar ou continuar.
- `R`: reiniciar depois de Game Over ou vitoria.
- `Esc`: sair.

A cobra se move automaticamente. Apenas uma mudanca de direcao e aceita entre
dois movimentos, e uma inversao direta e recusada.

## Regras implementadas

- A cobra comeca com tres quadrados.
- As quatro bordas causam Game Over quando tocadas.
- A comida surge aleatoriamente somente em celulas livres.
- Colisoes com o corpo encerram a partida.
- Cada comida vale um ponto e acrescenta um segmento.
- A pontuacao aparece durante a partida e na tela de Game Over.
- A velocidade aumenta a cada cinco pontos. O intervalo cai mais rapidamente
  nos dez primeiros niveis e continua diminuindo em todos os niveis restantes.

## Estrutura

- `src/game.c` e `src/game.h`: estado e regras independentes da interface.
- `src/main.c`: janela, desenho OpenGL, entrada e timer do FreeGLUT.
- `tests/test_game.c`: testes automatizados das regras centrais.
- `RELATORIO.md`: relatorio curto para a entrega.
