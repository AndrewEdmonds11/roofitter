#!/bin.bash
# Make sure each example runs

#roofitter -c Main/fcl/example.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_cemDio_mom.C("val-run7.root", "ana_cemDio_mom")'

#roofitter -c Main/fcl/example_crv.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_cemDioCrv_mom.C("val-run7.root")'

#roofitter -c Main/fcl/example_two_analyses.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_cemDio_mom.C("val-run7.root")'
#root -l 'Main/scripts/plot_cemDioCrv_mom.C("val-run7.root")'

#roofitter -c Main/fcl/example_momresp.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_crv_momresp.C("val-run7.root")'

#roofitter -c Main/fcl/example_unfold.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
#root -l 'Main/scripts/plot_cemDio_mom_unfold.C("val-run7.root")'

#roofitter -c Main/fcl/example_constrain.fcl -i /mu2e/data/users/edmonds/Offline_muepos/trkana-ensemble-run7i.root -t TrkAnaNeg/trkana -o val-run7.root
root -l 'Main/scripts/plot_cemDio_mom.C("val-run7.root", "ana_cemDio_mom_constrain")'
