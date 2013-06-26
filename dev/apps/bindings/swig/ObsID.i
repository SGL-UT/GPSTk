%include "../../../src/ObsID.hpp"


%pythoncode %{
ObsID.__str__ = lambda self: asString(self)

import __builtin__
class ObservationTypes:
    (Unknown, Any, Range, Phase, Doppler, SNR, Channel, Iono,
    SSI, LLI, TrackLen, NavMsg, Undefined, Last) = range(14)

class CarrierBands:
   (Unknown, Any , Zero, L1, L2, L5, G1, G2, E5b, E5ab, E1, 
    E2, E6, C6, L1L2, Undefined, Last) = range(17)

class TrackingCodes:
   (Unknown, Any, CA, P, Y, W, N, D, M , C2M, C2L, 
   C2LM, I5, Q5, IQ5, GCA, GP, A, B, C, BC, ABC, IE5, 
   QE5, IQE5, SCA, SI5, SQ5, SIQ5, CI2, CQ2, CIQ2, CI5, 
   CQ5, CIQ5, CI6, CQ6, CIQ6 , Undefined, Last) = range(40)

%}
