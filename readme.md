This is just a simple program to detect the level of traffic jam on Google Map.

Let's KISS again. Neural networks are not used. Instead, the algorithm is as follows:
1. convert the image into HSV
2. filter the image by some colour ranges
3. apply Hough transforms on the filtered image to extract lines 
4. determine the level of traffic jam by counting the amount of each type of lines (not implemented in the code)

For example, suppose we have a region on Google Map:
[Screen capture of a region on Google Map](sample-traffic-3.png)

The result of line detection:
[Result of line detection](result-sample-traffic-3.png)

Build:
```bash
gitclone https://github.com/tklam/google-map-traffic-analysis.git
cd google-map-traffic-analysis
mkdir build
cmake ..
```
