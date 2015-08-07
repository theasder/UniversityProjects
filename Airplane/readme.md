You can build it at least on Linux distributions or Windows.

To build project, use `make all`.
You should have file `task2.o` or `task2.exe` in `build/bin`.
To train model, you should write this:
`task2.exe -d ../../data/binary/train_labels.txt -m model.txt --train`
Though you should have images in data/binary so it will be trained on them.
To classify images, you should write this:
`task2.exe -d ../../data/binary/test_labels.txt -m model.txt -l predictions.txt --predict`
And to compare results with actual classification, write this:
`./compare.py data/binary/test_labels.txt build/bin/predictions.txt`


