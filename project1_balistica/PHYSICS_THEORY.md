# Physics Theory — Project 1: Ballistics and Projectile Launching

## Objective

This project introduces the numerical simulation of two-dimensional motion under gravity, with and without air resistance. Its teaching goal is to connect:

1. two-dimensional kinematics;
2. Newton's second law;
3. numerical integration of ordinary differential equations;
4. extraction of physical metrics such as flight time, horizontal range, and apex.

## Conventions

- length: meter (m)
- time: second (s)
- mass: kilogram (kg)
- velocity: m/s
- acceleration: m/s²
- launch angle in the interface: degree (deg)
- angle in the integrator: radian (rad)

In the physical domain, the $x$ axis increases to the right and the $y$ axis increases upward.

## Case without air resistance

When gravity acts alone, the acceleration is constant:

$$
\vec{a} = (0, -g)
$$

If the initial velocity has magnitude $v_0$ and angle $\theta$, then:

$$
v_{0x} = v_0\cos\theta
$$

$$
v_{0y} = v_0\sin\theta
$$

The kinematic equations are:

$$
x(t) = v_{0x} t
$$

$$
y(t) = v_{0y} t - \frac{1}{2}gt^2
$$

### Flight time

For launch and landing at the same level:

$$
T = \frac{2v_0\sin\theta}{g}
$$

### Horizontal range

$$
R = v_{0x} T = \frac{v_0^2\sin(2\theta)}{g}
$$

### Maximum height

The apex occurs when $v_y = 0$:

$$
t_{apice} = \frac{v_0\sin\theta}{g}
$$

and the maximum height is:

$$
h_{max} = \frac{v_0^2\sin^2\theta}{2g}
$$

These expressions serve as analytical references for validating numerical integration in the drag-free case.

## Case with air resistance

In the current project, air resistance is modeled as quadratic drag. The drag force points opposite to the velocity:

$$
\vec{F}_{drag} = -\frac{1}{2}\rho C_d A \|\vec{v}\|\vec{v}
$$

where:

- $\rho$ is the air density;
- $C_d$ is the drag coefficient;
- $A$ is the cross-sectional area;
- $\vec{v}$ is the instantaneous velocity.

Because the projectile is treated as a sphere, the area used is:

$$
A = \pi r^2
$$

Dividing the force by the mass $m$ gives the drag acceleration:

$$
\vec{a}_{drag} = -\frac{1}{2m}\rho C_d A \|\vec{v}\|\vec{v}
$$

The total acceleration is therefore:

$$
\vec{a} = \vec{a}_{gravidade} + \vec{a}_{drag}
$$

with

$$
\vec{a}_{gravidade} = (0, -g)
$$

This term makes the problem nonlinear and removes the simple closed-form solution of the ideal case. Numerical integration therefore becomes essential.

## Formulation as a system of ODEs

The dynamic state of the projectile is:

$$
\mathbf{s} = (x, y, v_x, v_y)
$$

Its time derivative is:

$$
\dot{\mathbf{s}} = (v_x, v_y, a_x, a_y)
$$

With drag, $a_x$ and $a_y$ depend on the instantaneous velocity, so the integrator must re-evaluate the derivative field during the step.

## Numerical methods used

### Symplectic Euler

The symplectic method updates the velocities first and then the position using the already corrected velocity. Schematically:

$$
\vec{v}_{n+1} = \vec{v}_n + \vec{a}(\mathbf{s}_n)\Delta t
$$

$$
\vec{x}_{n+1} = \vec{x}_n + \vec{v}_{n+1}\Delta t
$$

It is simple and inexpensive, and it often preserves the qualitative behavior of mechanical systems better than traditional explicit Euler.

### Fourth-order Runge-Kutta (RK4)

RK4 uses four intermediate evaluations per step:

$$
\mathbf{s}_{n+1} = \mathbf{s}_n + \frac{\Delta t}{6}(k_1 + 2k_2 + 2k_3 + k_4)
$$

with each $k_i$ obtained from derivatives evaluated at intermediate states. This method significantly reduces local error and is the project's default integrator.

## Ground impact detection

The ground is at $y = 0$. The current project ends the simulation at the first ground impact. This implementation has neither restitution nor bouncing after the collision.

Because the integrator works in discrete steps, the state can cross the ground between two samples. To keep the range from depending too heavily on the step size, the project uses cubic Hermite interpolation within the last step to locate the impact instant more precisely.

In practice, this substantially improves:

- the flight-time estimate;
- the final-range estimate;
- test stability when $\Delta t$ varies.

## Extracted physical metrics

The project records and displays:

- global simulation time;
- current or final horizontal range;
- apex point;
- time to apex;
- a history of unique launches with their parameters.

The apex is estimated from the recorded trajectory samples using local quadratic interpolation over neighboring points.

## Relevant physical validation

### Without drag

- compare the numerical range with the analytical expression $R = v_0^2\sin(2\theta)/g$;
- compare the flight time with the corresponding analytical expression;
- check the apex against the closed-form kinematic solution.

### With drag

- compare stability between coarser and finer time steps;
- verify that the solution remains physically plausible;
- compare different integrators to understand cost versus accuracy.

## Assumptions and simplifications of the current model

- the projectile is treated as a particle whose radius is used only to compute the cross-sectional area;
- projectile spin is not modeled;
- there is no Magnus effect;
- wind is not modeled;
- the ground is a rigid boundary at $y=0$;
- impact ends the trajectory instead of simulating a collision with restitution.

These simplifications make the project an ideal first numerical laboratory: it already requires integration, impact interpolation, and comparison between ideal and dissipative models, without introducing excessive complexity too early.
