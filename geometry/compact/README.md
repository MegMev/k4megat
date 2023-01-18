# dd4hep compact model #
These geometry models are based on dd4hep's compact xml format.
The models are driven by a few designing constants defined in the file, which are self-explanatory.
You can tune the detector design by tuning these parameters.

Use the following commands to check your new geometry model:
```bash
geoWebDisplay ../geometry/compact/Megat.xml

```
You may need to set the max vis node to 0 to show the model if the segmentation is very small.
