
/**
 * specify typemaps for outputs passed as pointers...
 * @todo: modify docstrings for functions that have multiple outputs
 */

%include "typemaps.i"
%apply double& OUTPUT {double& sf};
%apply double& OUTPUT {double& nadir, double& azimuth};
%apply double& OUTPUT {double& beta, double& phi};
%apply double& OUTPUT {double& yawrate};
