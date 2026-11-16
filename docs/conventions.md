# Frame Conventions (write before coding; never mix)

| Decision | Choice |
|---|---|
| Sky frame | x = vernal equinox, z = north celestial pole (ICRS/J2000) |
| Camera z | out of the lens (boresight) |
| Camera x | right in the image (columns increase) |
| Camera y | down in the image (rows increase) |
| Handedness | right-handed (x cross y = z) |
| Rotation | `A_cam_from_sky` takes sky components to camera components |
| Angles in code | radians (convert only for display) |
| Floating point | `double` everywhere |

## Naming
`v_cam`, `v_sky`, `A_cam_from_sky`. Frame names must cancel:
`v_cam = A_cam_from_sky * v_sky`.



