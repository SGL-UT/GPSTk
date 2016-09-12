#Tests for ClockModel Classes

add_executable(ObsClockModel_T ObsClockModel_T.cpp)
target_link_libraries(ObsClockModel_T gpstk)
add_test(ClockModel_ObsClockModel ObsClockModel_T)

# add_executable(EpochClockModel_T EpochClockModel_T.cpp)
# target_link_libraries(EpochClockModel_T gpstk)
# add_test(ClockModel_EpochClockModel EpochClockModel_T)

# add_executable(ObsEpochMap_T ObsEpochMap_T.cpp)
# target_link_libraries(ObsEpochMap_T gpstk)
# add_test(ClockModel_ObsEpochMap ObsEpochMap_T)

add_executable(ObsRngDev_T ObsRngDev_T.cpp)
target_link_libraries(ObsRngDev_T gpstk)
add_test(ClockModel_ObsRngDev ObsRngDev_T)

# add_executable(ORDEpoch_T ORDEpoch_T.cpp)
# target_link_libraries(ORDEpoch_T gpstk)
# add_test(ClockModel_ORDEpoch ORDEpoch_T)