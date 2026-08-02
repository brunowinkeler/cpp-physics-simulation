# Physics Theory — Project 2: Simple and Double Pendulums

## Objective

This project studies rotational oscillating systems using angular coordinates instead of direct Cartesian coordinates. It focuses on three areas:

1. rotational dynamics;
2. numerical integration of ordinary differential equations;
3. the transition between regular and chaotic behavior.

## Conventions

- Length unit: meter (m)
- Mass unit: kilogram (kg)
- Time unit: second (s)
- Angle unit in the interface: degree (deg)
- Angle unit in the integrator: radian (rad)
- Gravity: m/s²
- Angular velocity: rad/s

The angle $\theta = 0$ represents the rod pointing downward in stable equilibrium. Positive values rotate counterclockwise when viewed in the conventional Cartesian plane.

## Simple pendulum

### Model

The simple pendulum is modeled as a point mass attached to a rigid, massless rod of length $L$.

Its exact equation of motion is:

$$
\ddot{\theta} = -\frac{g}{L}\sin(\theta)
$$

The project also uses a linear damping term proportional to angular velocity:

$$
\ddot{\theta} = -\frac{g}{L}\sin(\theta) - c\dot{\theta}
$$

where:

- $g$ is gravity;
- $L$ is the rod length;
- $c$ is the angular damping coefficient.

### Small-angle approximation

When $|\theta| \ll 1$ rad, $\sin(\theta) \approx \theta$, and the equation becomes linear:

$$
\ddot{\theta} + \frac{g}{L}\theta = 0
$$

In this regime, the theoretical period is:

$$
T \approx 2\pi\sqrt{\frac{L}{g}}
$$

This expression is used as a numerical validation reference in the tests.

### Mechanical energy

For mass $m$, the simple pendulum's total energy can be written as:

$$
E = \frac{1}{2}mL^2\omega^2 + mgL\left(1 - \cos(\theta)\right)
$$

where $\omega = \dot{\theta}$.

Without damping, the energy should remain approximately constant. With damping, it should decrease over time.

## Double pendulum

### Model

The double pendulum has two point masses, $m_1$ and $m_2$, coupled by two rigid rods of lengths $L_1$ and $L_2$. Its degrees of freedom are $\theta_1$ and $\theta_2$.

The exact equations are nonlinear and coupled, and they depend simultaneously on the angles and angular velocities. One standard form for the angular accelerations is:

$$
\ddot{\theta}_1 = \frac{-g(2m_1+m_2)\sin\theta_1 - m_2g\sin(\theta_1 - 2\theta_2) - 2\sin(\theta_1-\theta_2)m_2\left(\omega_2^2L_2 + \omega_1^2L_1\cos(\theta_1-\theta_2)\right)}{L_1\left(2m_1+m_2-m_2\cos(2\theta_1-2\theta_2)\right)}
$$

$$
\ddot{\theta}_2 = \frac{2\sin(\theta_1-\theta_2)\left(\omega_1^2L_1(m_1+m_2) + g(m_1+m_2)\cos\theta_1 + \omega_2^2L_2m_2\cos(\theta_1-\theta_2)\right)}{L_2\left(2m_1+m_2-m_2\cos(2\theta_1-2\theta_2)\right)}
$$

The project also adds linear damping terms:

$$
\ddot{\theta}_1 \leftarrow \ddot{\theta}_1 - c_1\omega_1
$$

$$
\ddot{\theta}_2 \leftarrow \ddot{\theta}_2 - c_2\omega_2
$$

### Mechanical energy

The mass positions are:

$$
x_1 = L_1\sin\theta_1, \qquad y_1 = -L_1\cos\theta_1
$$

$$
x_2 = x_1 + L_2\sin\theta_2, \qquad y_2 = y_1 - L_2\cos\theta_2
$$

The velocities are obtained by differentiating these expressions. The total energy is:

$$
E = \frac{1}{2}m_1(v_{x1}^2 + v_{y1}^2) + \frac{1}{2}m_2(v_{x2}^2 + v_{y2}^2) + U
$$

with gravitational potential measured relative to the lowest-energy configuration:

$$
U = m_1gL_1(1-\cos\theta_1) + m_2g\left(L_1(1-\cos\theta_1) + L_2(1-\cos\theta_2)\right)
$$

## Numerical integration

The project implements two integrators:

### Symplectic Euler

- Updates the angular velocities first;
- Then updates the angles using the corrected velocities.

It is simple and inexpensive, and it often preserves the system's qualitative structure better than standard explicit Euler.

### Fourth-order Runge-Kutta (RK4)

It uses four intermediate evaluations per time step and produces much lower local error than first-order methods.

For the simple pendulum, RK4 should closely approximate the theoretical small-angle period. For the double pendulum, it helps reduce local error in a system that is highly sensitive to initial conditions.

## Chaos and sensitivity to initial conditions

The double pendulum is a classic example of a deterministic system that can exhibit chaotic behavior. This means that:

- the equations are fully deterministic;
- small differences in the initial state can grow rapidly over time;
- long-term predictions become highly sensitive to numerical error.

Therefore, double-pendulum tests should prioritize:

- absence of `NaN` and `Inf`;
- numerical stability for reasonable time steps;
- basic invariants when the system has no damping;
- simple special cases, such as exact rest at $\theta_1 = \theta_2 = 0$.

## Practical validation targets

### Simple pendulum

- small-angle period;
- approximate energy conservation without damping;
- energy decay with damping;
- stability with different integration methods.

### Double pendulum

- stable rest in the downward vertical configuration;
- finite evolution without numerical blow-up;
- coherent trails for both bobs;
- qualitatively distinct behavior between regular and chaotic regimes.

## Model limitations

This project deliberately simplifies several aspects:

- rods are rigid and massless;
- there is no pivot friction beyond the model's imposed linear damping;
- masses and rods do not collide;
- there is no distributed air resistance along the rods;
- the problem is two-dimensional.

These simplifications are appropriate for studying numerical integration, energy, periodicity, and chaos without introducing unnecessary complexity too early.
