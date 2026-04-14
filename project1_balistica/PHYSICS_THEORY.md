# Teoria Física — Projeto 1: Balística e Lançamento de Projéteis

## Objetivo

Este projeto introduz a simulação numérica de movimento bidimensional sob gravidade, com e sem resistência do ar. O objetivo didático é ligar:

1. cinemática em 2D;
2. segunda lei de Newton;
3. integração numérica de EDOs;
4. extração de métricas físicas como tempo de voo, alcance horizontal e ápice.

## Convenções adotadas

- comprimento: metro (m)
- tempo: segundo (s)
- massa: quilograma (kg)
- velocidade: m/s
- aceleração: m/s²
- ângulo de lançamento na interface: grau (deg)
- ângulo no integrador: radiano (rad)

No domínio físico, o eixo $x$ cresce para a direita e o eixo $y$ cresce para cima.

## Caso sem resistência do ar

Quando só a gravidade atua, a aceleração é constante:

$$
\vec{a} = (0, -g)
$$

Se a velocidade inicial tem módulo $v_0$ e ângulo $\theta$, então:

$$
v_{0x} = v_0\cos\theta
$$

$$
v_{0y} = v_0\sin\theta
$$

As equações cinemáticas são:

$$
x(t) = v_{0x} t
$$

$$
y(t) = v_{0y} t - \frac{1}{2}gt^2
$$

### Tempo de voo

Para lançamento e aterrissagem no mesmo nível:

$$
T = \frac{2v_0\sin\theta}{g}
$$

### Alcance horizontal

$$
R = v_{0x} T = \frac{v_0^2\sin(2\theta)}{g}
$$

### Altura máxima

O instante do ápice ocorre quando $v_y = 0$:

$$
t_{apice} = \frac{v_0\sin\theta}{g}
$$

e a altura máxima é:

$$
h_{max} = \frac{v_0^2\sin^2\theta}{2g}
$$

Essas expressões servem como referências analíticas para validar a integração numérica no caso sem arrasto.

## Caso com resistência do ar

No projeto atual, a resistência do ar é modelada por arrasto quadrático. A força de arrasto aponta na direção oposta à velocidade:

$$
\vec{F}_{drag} = -\frac{1}{2}\rho C_d A \|\vec{v}\|\vec{v}
$$

onde:

- $\rho$ é a densidade do ar;
- $C_d$ é o coeficiente de arrasto;
- $A$ é a área de seção transversal;
- $\vec{v}$ é a velocidade instantânea.

Como o projétil é tratado como esfera, a área usada é:

$$
A = \pi r^2
$$

Dividindo a força pela massa $m$, obtém-se a aceleração de arrasto:

$$
\vec{a}_{drag} = -\frac{1}{2m}\rho C_d A \|\vec{v}\|\vec{v}
$$

Logo, a aceleração total é:

$$
\vec{a} = \vec{a}_{gravidade} + \vec{a}_{drag}
$$

com

$$
\vec{a}_{gravidade} = (0, -g)
$$

Esse termo torna o problema não linear e elimina a solução fechada simples do caso ideal. Por isso, a integração numérica se torna essencial.

## Formulação como sistema de EDOs

O estado dinâmico do projétil é:

$$
\mathbf{s} = (x, y, v_x, v_y)
$$

Sua derivada temporal é:

$$
\dot{\mathbf{s}} = (v_x, v_y, a_x, a_y)
$$

No caso com arrasto, $a_x$ e $a_y$ dependem da velocidade instantânea, então o integrador precisa reavaliar o campo derivado ao longo do passo.

## Métodos numéricos usados

### Symplectic Euler

O método simplético atualiza primeiro as velocidades e depois a posição com a velocidade já corrigida. Em forma esquemática:

$$
\vec{v}_{n+1} = \vec{v}_n + \vec{a}(\mathbf{s}_n)\Delta t
$$

$$
\vec{x}_{n+1} = \vec{x}_n + \vec{v}_{n+1}\Delta t
$$

É simples e barato, e muitas vezes preserva melhor o comportamento qualitativo de sistemas mecânicos do que o Euler explícito tradicional.

### Runge-Kutta de 4ª ordem (RK4)

O RK4 usa quatro avaliações intermediárias por passo:

$$
\mathbf{s}_{n+1} = \mathbf{s}_n + \frac{\Delta t}{6}(k_1 + 2k_2 + 2k_3 + k_4)
$$

com cada $k_i$ obtido a partir de derivadas avaliadas em estados intermediários. Esse método reduz significativamente o erro local e é o integrador padrão do projeto.

## Detecção de impacto com o solo

O solo está em $y = 0$. O projeto atual encerra a simulação no primeiro impacto com o solo. Não há, nesta implementação, restituição nem ricochete após a colisão.

Como o integrador trabalha por passos discretos, o estado pode atravessar o solo entre duas amostras. Para evitar que o alcance dependa demais do tamanho de passo, o projeto usa interpolação cúbica de Hermite dentro do último passo para localizar o instante de impacto de forma mais precisa.

Em termos práticos, isso melhora bastante:

- a estimativa do tempo de voo;
- a estimativa do alcance final;
- a estabilidade dos testes ao variar $\Delta t$.

## Métricas físicas extraídas

O projeto registra e exibe:

- tempo global de simulação;
- alcance horizontal atual ou final;
- ponto de ápice;
- tempo até o ápice;
- histórico de lançamentos únicos com seus parâmetros.

O ápice é estimado a partir das amostras registradas da trajetória usando interpolação quadrática local sobre pontos vizinhos.

## Validação física relevante

### Sem arrasto

- comparar o alcance numérico com a expressão analítica $R = v_0^2\sin(2\theta)/g$;
- comparar o tempo de voo com a expressão analítica correspondente;
- verificar o ápice com a solução fechada da cinemática.

### Com arrasto

- comparar estabilidade entre passos de tempo mais grossos e mais finos;
- verificar se a solução permanece fisicamente plausível;
- comparar diferentes integradores para entender custo versus precisão.

## Hipóteses e simplificações do modelo atual

- o projétil é tratado como partícula com raio usado apenas no cálculo da seção transversal;
- a rotação do projétil não é modelada;
- não há efeito Magnus;
- o vento não é modelado;
- o solo é um limite rígido em $y=0$;
- o impacto encerra a trajetória em vez de simular colisão com restituição.

Essas simplificações tornam o projeto ideal como primeiro laboratório numérico: ele já exige integração, interpolação de impacto e comparação entre modelos ideal e dissipativo, mas sem introduzir complexidade excessiva cedo demais.
