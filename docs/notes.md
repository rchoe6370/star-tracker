# Notes Log

## CSV contracts
| File | Written by | Read by | Columns |
|---|---|---|---|
| data/catalog.csv | prepare_catalog.py | C++ catalog | star_id,ra_deg,dec_deg,magnitude |
| data/results/attitude_sweep.csv | experiment_attitude | plot_attitude.py | trial,method,noise_px,num_stars,error_arcsec,roll_error_arcsec |
| data/results/identify_sweep.csv | experiment_identify | plot_identify.py | trial,noise_px,false_stars,missing_prob,fov_deg,success,error_arcsec,runtime_ms |
| data/results/solved_image.csv | solve_image | plot_overlay.py | u,v,star_id,residual_px |

Rules: header row always; one row per trial; save seed + settings in a .txt next to each CSV.
s
## Log

