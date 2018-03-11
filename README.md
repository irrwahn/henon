# Hénon

Visualization fun with Hénon maps.

## Background

The inspiration for this visualization arose from reading a paper about
discrete-time dynamical systems and strange attractors. Unfortunately
I lost any reference to that specific paper. However, the images are
generated by iterating a configured number of times over the following
map equation taken from said paper:

        y_n+1 := x_n * sin(a) + (y_n - x_n^2) * cos(a)
        x_n+1 := x_n * cos(a) - (y_n - x_n^2) * sin(a)

For a given parameter `a` this is repeated for a range of initial values
`v`: `x_0 = y_0 = v`. Depending on the choice of parameter `a`, the
system will produce closed graphs for a certain range of initial values,
yet tip over into chaotic behavior beyond some specific limit. This
usually happens by going through transition states characterized by the
closed graph fragmenting into multiple disjunct self-similar divisions.
This whole process is repeated for different values for the parameter
`a` by sweeping over a configured range.

A separate output image is generated for each discrete value of `a`, i.e.
each image contains the iterative mappings over the configured range of
start values for a constant `a`. A simple RGB color palette mechanism is
used to make the distinct iterative sets discernable and add a touch of
visual appeal to the images.

## History

The first incarnation of this code was written in the late 1980s for a
i286 DOS machine with Hercules graphics. While the first versions used
direct graphics output, the current version is a 2016 full rewrite in
pure ISO C that simply generates a set of bitmap files in either PPM or
PNG format that can subsequently be processed by whatever means seem
appropriate.

## Build

Simply run

        make

to build the `henon` executable using the default configuration. The
only external build dependency is on `libpng`. However, you can skip it
by running the build command like this:

        USE_PNG=0 make

In this case the PNG image output format will not be available, but
the built-in PPM format support still is.

## Usage

Running `henon -h` will display this short help text:

    Usage: henon [options] [-c paramfile] [-p palettefile] [-o output_prefix]
      -c : Set parameter file path; default: none.
      -o : Set output path prefix; default: .
      -i : Write an info file for each output file; default: no.
      -p : Set color palette file path, first entry is background color;
           default: none.
      -f : Flip color palette.
      -n : Generate PPM image output (the default).
      -N : Generate PNG image output.
      -v : Increase verbosity, show progress.
      -h : Display this help text and exit.

You probably want to at least specify an output path prefix via the `-o`
option to not clutter up your current working directory with the slew of
image files produced. Note that depending on the machine and the set
of parameters the image generation may take a while.

The `data` subdirectory contains several sample configuration files,
each defining the following set of parameters:

        a0    # start value for parameter 'a' sweep
        a1    # end value for parameter 'a' sweep
        as    # parameter 'a' sweep step width
        v0    # initial iteration value range start
        v1    # initial iteration value range end
        vs    # initial iteration value step width
        iter  # number of iterations for each initial value
        xdim  # horizontal dimension of output image
        ydim  # vertical dimension of output image
        xoff  # x display offset
        yoff  # y display offset
        zoom  # display zoom factor

You are strongly encouraged to play around with these parameters to
explore the characteristics of the results produced by the mapping
algorithm, the sample files may give you some hints towards sensible
combinations. The number of images generated is determined by the sweep
range divided by the step width, i.e. `(a1-a0)/as`.

The `pal` directory contains sample RGB color palette files. Each line
starting with an optional `#` character followed by hexadecimal digits
is interpreted as a 24 bit RGB color definition in `#rrggbb` format.

## Helper scripts

The `annotate.sh` and `mkvid.sh` helper scripts demonstrate how to
process the generated images to add parameter annotations, and how to
assemble a batch of images into an animated GIF, or a AVI or MP4/MKV
video sequence. Note that these scripts require the following external
tools to be available on the runtime system:

        imagemagick  # convert utility used for image annotation
        ffmpeg       # GIF, AVI, MP4, MKV generation
        mpv          # optional, only used for video playback

The `demo.sh` script performs all steps from image generation to video
assembly using a sane set of default parameters.

## License

Henon is distributed under the Modified ("3-clause") BSD License. See
`LICENSE` file for more information.

----------------------------------------------------------------------
