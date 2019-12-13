#!/bin.bash
# Make sure each example runs

#roofitter -c Main/fcl/example.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_example.C("val-run7.root")'

#roofitter -c Main/fcl/example_crv.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_example_crv.C("val-run7.root")'

#roofitter -c Main/fcl/example_two_analyses.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_example_two_analyses.C("val-run7.root")'

#roofitter -c Main/fcl/example_momresp.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_example_momresp.C("val-run7.root")'

#roofitter -c Main/fcl/example_unfold.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_example_unfold.C("val-run7.root")'

#roofitter -c Main/fcl/example_constrain.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run1004i.root -t TrkAnaNeg/trkana -o val-run1004.root
root -l 'Main/scripts/plot_example_constrain.C("val-run1004.root")'
