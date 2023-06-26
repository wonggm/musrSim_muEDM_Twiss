# musrSim_muEDM_Twiss
musrSim modified with Geant4 wrapper (https://github.com/kyrsjo/Miniscatter), enabling beam optics parameter inputs for simulation。\
\
Errors when building musrSim, particularly those of "namespace", can be resolve by checking and setting CMAKE version：
```
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

# Update Spring 2023
### Files changed: 
`musrSim.cc`\
in `src/`,\
**`musrPrimaryGeneratorAction.cc`**, `musrPrimaryGeneratorMessenger.cc`\
and in respective counterpart of header files \
\
**`musrPrimaryGeneratorAction.cc`**:
- **line 144 - 167**: reads Twiss parameters input from `.mac` file
- **line 169 - 249**: set-up covariance matrix from the given Twiss parameters
- **line 537 - 566**: a condition calculating the position and momentum components (x, px, y, py), using the covariance matrix elements calculated
- For Twiss parameters input with <em>emittance, &epsilon;</em> in units of &pi;, **line 624** and **line 630** can be uncommented (while commenting-out **line 625** and **line 631** respectively)
\
`musrPrimaryGeneratorMessenger.cc`:
- **line 43- 59**: parsing inputs of Twiss parameters to corresponding variables
- line 45: adds a new command that calls function related to Twiss parameter
