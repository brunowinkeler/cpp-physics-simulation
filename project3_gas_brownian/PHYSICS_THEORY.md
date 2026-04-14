# Teoria Física - Projeto 3: Gás Ideal e Movimento Browniano

## Objetivo

Este projeto introduz um laboratório 2D de partículas para explorar:

1. colisões elásticas em muitas partículas;
2. temperatura como medida da energia cinética média;
3. pressão como transferência de momento nas paredes;
4. movimento Browniano de uma partícula traçadora em um banho térmico.

## Convenções e limites do modelo

- O projeto usa uma caixa bidimensional retangular.
- As partículas são discos rígidos com colisões elásticas perfeitas.
- A massa por partícula é inspirada em gases reais, a partir da massa molar.
- O raio usado na colisão e no desenho é um raio efetivo didático, não o diâmetro molecular literal.
- A escala térmica do simulador é ajustada para legibilidade e estabilidade interativa.

Isso significa que o projeto preserva analogias físicas importantes, como a relação entre massa, agitação térmica e colisões, mas não pretende reproduzir quantitativamente um gás real em escala molecular absoluta.

## Estado de cada partícula

Cada partícula possui:

- posição $\mathbf{x} = (x, y)$;
- velocidade $\mathbf{v} = (v_x, v_y)$;
- massa $m$;
- raio efetivo $r$.

Durante o passo de tempo, a posição é atualizada por:

$$
\mathbf{x}_{n+1} = \mathbf{x}_n + \mathbf{v}_n \Delta t
$$

Como não há força contínua de longo alcance no v1, a dinâmica entre colisões é puramente inercial.

## Colisões com as paredes

As paredes da caixa refletem elasticamente as partículas. Quando uma partícula atinge uma parede, a componente normal da velocidade troca de sinal.

Por exemplo, em uma parede vertical:

$$
v_x' = -v_x
$$

e em uma parede horizontal:

$$
v_y' = -v_y
$$

O impulso transferido à parede é acumulado para estimar a pressão instantânea.

## Colisões entre partículas

Quando dois discos se sobrepõem, o resolvedor usa o vetor normal da linha entre seus centros e aplica a fórmula padrão de colisão elástica ao longo dessa normal.

Para um coeficiente de restituição $e = 1$:

$$
j = -\frac{(1 + e)(\mathbf{v}_{rel} \cdot \mathbf{n})}{\frac{1}{m_1} + \frac{1}{m_2}}
$$

com $\mathbf{n}$ sendo a normal de contato e $\mathbf{v}_{rel}$ a velocidade relativa.

Esse impulso atualiza as velocidades e uma correção posicional remove a sobreposição geométrica residual.

## Temperatura efetiva

Em 2D, a energia cinética translacional média por partícula é usada como base para uma temperatura efetiva:

$$
\langle E_k \rangle = \frac{1}{N}\sum_{i=1}^{N} \frac{1}{2} m_i \|\mathbf{v}_i\|^2
$$

e então:

$$
T_{eff} = \frac{\langle E_k \rangle}{k_{eff}}
$$

onde $k_{eff}$ é uma constante térmica efetiva do simulador. Ela preserva relações qualitativas entre massa e agitação, mas não pretende ser a constante de Boltzmann em uma escala molecular literal.

## Pressão instantânea

A pressão do v1 é tratada como uma grandeza instantânea derivada do impulso total transferido às paredes dentro de uma janela temporal deslizante.

Se $\Delta p_{wall}$ é o impulso total acumulado nas paredes durante a janela e $\Delta t_{window}$ sua duração efetiva, então a grandeza exibida é:

$$
P_{inst} = \frac{\Delta p_{wall}}{\Delta t_{window} L_{wall}}
$$

onde $L_{wall}$ é o perímetro da caixa.

Em um sistema 2D, essa quantidade deve ser lida como uma analogia de carga de parede por unidade de comprimento, útil para comparação qualitativa entre cenários.

## Movimento Browniano

No modo Browniano, uma única partícula traçadora recebe massa e raio maiores que as partículas do banho térmico. Ela sofre colisões sucessivas com muitas partículas menores, produzindo uma trajetória irregular e difusiva.

O projeto registra apenas a trajetória dessa partícula destacada, e o usuário pode desligar esse registro para reduzir custo de atualização e desenho.

## Estratégia numérica do v1

- integração por passo fixo;
- colisões elásticas resolvidas diretamente;
- malha uniforme para reduzir o custo de detecção de pares;
- histórico temporal de pressão em janela deslizante;
- trilha da partícula traçadora em janela deslizante.

## Validação esperada

O v1 deve validar pelo menos:

- conservação de momento linear em colisões partícula-partícula;
- conservação de energia cinética nas colisões elásticas dentro das tolerâncias numéricas;
- reflexão correta em paredes;
- estabilidade do plot temporal de pressão sem degradação progressiva;
- trajetória Browniana recente densa e opcional.
