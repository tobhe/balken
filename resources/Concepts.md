### Image: blaze Matrix ( .rows(), .columns() ...)

**Transform:**
```
convolve(Image, Kernel) -> Image
close(Image) -> Image
open(Image) -> Image
dilate(Image) -> Image
erode(Image) -> Image
```

**Conversion:**
```
make_normalized(Image) -> GreyImage
histogram::stretch(Image) -> GreyImage
```
### GreyImage: Image

**Construct:**
```
load_image(filepath) -> Image
```
**Conversion:**
```
fast_independent_scan(Image) -> DistanceMap
binarize(Image, threshold) -> BinaryImage
```
**View:**
```
view_image(Image) -> ImageViewer
compare(Image, Image) -> ImageViewer
```

### BWImage: Image
**Construct:**
```
binarize(Image, threshold) -> BWImage
```

**Conversion:**
```
from_image(Image) -> Code
find_regions(Image) -> vector(Region)
```
### DistanceMap: Image

**Construct:**
```
fast_independent_scan(Image) -> DistanceMap
```

**Conversion:**
```
prune(DistanceMap) -> BWImage
```

### Region: STL Container of Pixels

**Construct:**
```
find_regions(Image) -> vector(Region)

```

