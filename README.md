# MANGO at INAF simulation

- Working only with geant4 11.1.0 (no idea why)
  - following: https://geant4.web.cern.ch/download/release-notes/notes-v11.2.0.html There is a line of code in the main to add for v11.2 and next
    - `G4HadronicParameters::Instance()->SetTimeThresholdForRadioactiveDecay( 1.0e+60*CLHEP::year );`
  - comment it if you are unsing <=11.1.0