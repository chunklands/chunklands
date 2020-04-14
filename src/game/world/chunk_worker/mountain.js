
const Ax      = 24;
const omega_x = (2 * Math.PI) / 51;
const phi_x   = (2 * Math.PI) / 37;
const Az      = 37;
const omega_z = (2 * Math.PI) / 33;
const phi_z   = (2 * Math.PI) / 27;

module.exports = {
  isGround(x, y, z) {
    return y < (
        (Ax * Math.sin(omega_x * x + phi_x))
        + (Az * Math.sin(omega_z * z + phi_z))
    );
  }
};
