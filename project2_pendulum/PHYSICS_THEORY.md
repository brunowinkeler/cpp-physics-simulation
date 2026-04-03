# Teoria Física — Projeto 2: Pêndulo Simples e Pêndulo Duplo

## Objetivo

Este projeto estuda sistemas oscilatórios rotacionais usando coordenadas angulares em vez de coordenadas cartesianas diretas. O foco está em três pilares:

1. dinâmica rotacional;
2. integração numérica de equações diferenciais ordinárias;
3. transição entre comportamento regular e comportamento caótico.

## Convenções adotadas

- Unidade de comprimento: metro (m)
- Unidade de massa: quilograma (kg)
- Unidade de tempo: segundo (s)
- Unidade de ângulo na interface: grau (deg)
- Unidade de ângulo no integrador: radiano (rad)
- Gravidade: m/s²
- Velocidade angular: rad/s

O ângulo $\theta = 0$ representa a haste apontando para baixo, em equilíbrio estável. Valores positivos giram no sentido anti-horário quando observados no plano cartesiano tradicional.

## Pêndulo simples

### Modelo

O pêndulo simples é modelado como uma massa pontual presa a uma haste rígida e sem massa, com comprimento $L$.

Sua equação de movimento sem aproximações é:

$$
\ddot{\theta} = -\frac{g}{L}\sin(\theta)
$$

Neste projeto também é usado um termo de amortecimento linear proporcional à velocidade angular:

$$
\ddot{\theta} = -\frac{g}{L}\sin(\theta) - c\dot{\theta}
$$

onde:

- $g$ é a gravidade;
- $L$ é o comprimento da haste;
- $c$ é o coeficiente de amortecimento angular.

### Aproximação de pequeno ângulo

Quando $|\theta| \ll 1$ rad, vale $\sin(\theta) \approx \theta$ e a equação se torna linear:

$$
\ddot{\theta} + \frac{g}{L}\theta = 0
$$

Nesse regime, o período teórico é:

$$
T \approx 2\pi\sqrt{\frac{L}{g}}
$$

Essa expressão é usada como referência de validação numérica nos testes.

### Energia mecânica

Com massa $m$, a energia total do pêndulo simples pode ser escrita como:

$$
E = \frac{1}{2}mL^2\omega^2 + mgL\left(1 - \cos(\theta)\right)
$$

onde $\omega = \dot{\theta}$.

Sem amortecimento, a energia deve permanecer aproximadamente constante. Com amortecimento, ela deve decair ao longo do tempo.

## Pêndulo duplo

### Modelo

O pêndulo duplo possui duas massas pontuais $m_1$ e $m_2$ acopladas por duas hastes rígidas de comprimentos $L_1$ e $L_2$. Os graus de liberdade são $\theta_1$ e $\theta_2$.

As equações exatas são não lineares, acopladas e dependem simultaneamente dos ângulos e das velocidades angulares. Uma forma clássica para as acelerações angulares é:

$$
\ddot{\theta}_1 = \frac{-g(2m_1+m_2)\sin\theta_1 - m_2g\sin(\theta_1 - 2\theta_2) - 2\sin(\theta_1-\theta_2)m_2\left(\omega_2^2L_2 + \omega_1^2L_1\cos(\theta_1-\theta_2)\right)}{L_1\left(2m_1+m_2-m_2\cos(2\theta_1-2\theta_2)\right)}
$$

$$
\ddot{\theta}_2 = \frac{2\sin(\theta_1-\theta_2)\left(\omega_1^2L_1(m_1+m_2) + g(m_1+m_2)\cos\theta_1 + \omega_2^2L_2m_2\cos(\theta_1-\theta_2)\right)}{L_2\left(2m_1+m_2-m_2\cos(2\theta_1-2\theta_2)\right)}
$$

Neste projeto são adicionados também termos lineares de amortecimento:

$$
\ddot{\theta}_1 \leftarrow \ddot{\theta}_1 - c_1\omega_1
$$

$$
\ddot{\theta}_2 \leftarrow \ddot{\theta}_2 - c_2\omega_2
$$

### Energia mecânica

As posições das massas são:

$$
x_1 = L_1\sin\theta_1, \qquad y_1 = -L_1\cos\theta_1
$$

$$
x_2 = x_1 + L_2\sin\theta_2, \qquad y_2 = y_1 - L_2\cos\theta_2
$$

As velocidades são obtidas derivando essas expressões. A energia total é:

$$
E = \frac{1}{2}m_1(v_{x1}^2 + v_{y1}^2) + \frac{1}{2}m_2(v_{x2}^2 + v_{y2}^2) + U
$$

com potencial gravitacional relativo à configuração de menor energia:

$$
U = m_1gL_1(1-\cos\theta_1) + m_2g\left(L_1(1-\cos\theta_1) + L_2(1-\cos\theta_2)\right)
$$

## Integração numérica

O projeto implementa dois integradores:

### Symplectic Euler

- Atualiza primeiro as velocidades angulares;
- Depois atualiza os ângulos com as velocidades já corrigidas.

É simples, barato e muitas vezes preserva melhor a estrutura qualitativa do sistema do que o Euler explícito padrão.

### Runge-Kutta de 4ª ordem (RK4)

Usa quatro avaliações intermediárias por passo de tempo e produz erro local muito menor que métodos de primeira ordem.

No pêndulo simples, o RK4 deve se aproximar muito bem do período teórico para pequenos ângulos. No pêndulo duplo, ele ajuda a reduzir erro local em um sistema altamente sensível às condições iniciais.

## Caos e sensibilidade às condições iniciais

O pêndulo duplo é um exemplo clássico de sistema determinístico que pode apresentar comportamento caótico. Isso significa que:

- as equações são completamente determinísticas;
- pequenas diferenças no estado inicial podem crescer rapidamente com o tempo;
- previsões de longo prazo tornam-se muito sensíveis ao erro numérico.

Por isso, testes do pêndulo duplo devem priorizar:

- ausência de `NaN` e `Inf`;
- estabilidade numérica para passos de tempo razoáveis;
- invariantes básicos quando o sistema está sem amortecimento;
- casos especiais simples, como o repouso exato em $\theta_1 = \theta_2 = 0$.

## O que validar na prática

### Pêndulo simples

- período para pequenos ângulos;
- conservação aproximada de energia sem amortecimento;
- decaimento de energia com amortecimento;
- estabilidade para diferentes métodos de integração.

### Pêndulo duplo

- repouso estável na configuração vertical para baixo;
- evolução finita sem explosões numéricas;
- trilhas coerentes dos dois bobs;
- comportamento qualitativo distinto entre regimes simples e caóticos.

## Limites do modelo

Este projeto deliberadamente simplifica vários aspectos:

- hastes são rígidas e sem massa;
- não há atrito no pivô além do amortecimento linear imposto no modelo;
- não há colisão entre massas ou hastes;
- não há resistência do ar distribuída ao longo da haste;
- o problema é bidimensional.

Essas simplificações são adequadas para estudar integração numérica, energia, periodicidade e caos sem introduzir complexidade desnecessária cedo demais.
