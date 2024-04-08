# FluidSim2D
Simulation of a 2D Fluid based on the work of Jos Stam and his paper "Real Time Fluid Dynamics for Games" accessable under http://www.dgp.toronto.edu/people/stam/reality/index.html.

The main physical behaviour implemented in this code is the Navier Stokes equation

$\frac{\partial u}{\partial t} = - (u \cdot \nabla) u + \nu \nabla^2 u + f$

and the quation for smoke density

$\frac{\partial \rho}{\partial t} = - (u \cdot \nabla) \rho + \kappa \nabla^2 \rho + S$.
