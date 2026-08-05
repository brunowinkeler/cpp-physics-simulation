# Physics Theory - Project 3: Ideal Gas and Brownian Motion

## Objective

This project introduces a two-dimensional particle laboratory for exploring:

1. elastic collisions among many particles;
2. temperature as a measure of average kinetic energy;
3. pressure as momentum transfer at the walls;
4. Brownian motion of a tracer particle in a thermal bath.

## Conventions and model limits

- The project uses a rectangular two-dimensional box.
- Particles are rigid disks with perfectly elastic collisions.
- The mass per particle is inspired by real gases, derived from the molar mass.
- The radius used for collisions and drawing is a didactic effective radius, not the literal molecular diameter.
- The simulator's thermal scale is tuned for readability and interactive stability.

This means the project preserves important physical analogies, such as the relationship between mass, thermal agitation, and collisions, but it does not attempt to reproduce a real gas quantitatively at an absolute molecular scale.

## State of each particle

Each particle has:

- position $\mathbf{x} = (x, y)$;
- velocity $\mathbf{v} = (v_x, v_y)$;
- mass $m$;
- effective radius $r$.

During the time step, the position is updated by:

$$
\mathbf{x}_{n+1} = \mathbf{x}_n + \mathbf{v}_n \Delta t
$$

Because there is no continuous long-range force in v1, the dynamics between collisions is purely inertial.

## Collisions with the walls

The box walls reflect the particles elastically. When a particle hits a wall, the normal component of the velocity changes sign.

For example, at a vertical wall:

$$
v_x' = -v_x
$$

and at a horizontal wall:

$$
v_y' = -v_y
$$

The impulse transferred to the wall is accumulated to estimate the instantaneous pressure.

## Collisions between particles

When two disks overlap, the solver uses the normal vector of the line between their centers and applies the standard elastic collision formula along that normal.

For a restitution coefficient $e = 1$:

$$
j = -\frac{(1 + e)(\mathbf{v}_{rel} \cdot \mathbf{n})}{\frac{1}{m_1} + \frac{1}{m_2}}
$$

with $\mathbf{n}$ being the contact normal and $\mathbf{v}_{rel}$ the relative velocity.

This impulse updates the velocities, and a positional correction removes the residual geometric overlap.

## Effective temperature

In 2D, the average translational kinetic energy per particle is used as the basis for an effective temperature:

$$
\langle E_k \rangle = \frac{1}{N}\sum_{i=1}^{N} \frac{1}{2} m_i \|\mathbf{v}_i\|^2
$$

and then:

$$
T_{eff} = \frac{\langle E_k \rangle}{k_{eff}}
$$

where $k_{eff}$ is an effective thermal constant of the simulator. It preserves qualitative relationships between mass and agitation, but it is not intended to be the Boltzmann constant on a literal molecular scale.

## Instantaneous pressure

Pressure in v1 is treated as an instantaneous quantity derived from the total impulse transferred to the walls within a sliding time window.

If $\Delta p_{wall}$ is the total impulse accumulated at the walls during the window and $\Delta t_{window}$ is its effective duration, then the displayed quantity is:

$$
P_{inst} = \frac{\Delta p_{wall}}{\Delta t_{window} L_{wall}}
$$

where $L_{wall}$ is the perimeter of the box.

In a 2D system, this quantity should be read as an analogy for wall load per unit length, useful for qualitative comparison between scenarios.

## Brownian motion

In Brownian mode, a single tracer particle receives greater mass and radius than the thermal-bath particles. It undergoes successive collisions with many smaller particles, producing an irregular, diffusive trajectory.

The project records only the trajectory of that highlighted particle, and the user can turn that recording off to reduce update and drawing cost.

## Numerical strategy of v1

- fixed-step integration;
- elastic collisions resolved directly;
- uniform grid to reduce the cost of pair detection;
- pressure time history in a sliding window;
- tracer particle trail in a sliding window.

## Expected validation

v1 must validate at least:

- conservation of linear momentum in particle-particle collisions;
- conservation of kinetic energy in elastic collisions within numerical tolerances;
- correct reflection at walls;
- stability of the pressure time plot without progressive degradation;
- a dense and optional recent Brownian trajectory.
