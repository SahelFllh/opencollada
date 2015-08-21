# Basics #
In contrast to other existing COLLADA exporters, this new plug-ins do not store the
COLLADA document in an intermidiate data model but writes it directly to file. This leads to a dramatic reduction of memory consumption and to much a better performance.

# Directories #

|COLLADABaseUtils|Utils used by many of the other projects|
|:---------------|:---------------------------------------|
|COLLADAFramework|Datamodel used to load COLLADA files    |
|COLLADAMax      |OpenCOLLADA for Max plug-in sources     |
|COLLADAMaya     |OpenCOLLADA for Maya plug-in sources    |
|COLLADAStreamWriter|COLLADAStreamWriter sources (Library to write COLLADA files)|
|COLLADASaxFrameworkLoader|Library that loads COLLADA files in a SAX like manner into the framework data model|
|COLLADAValidator|XML validator for COLLADA files, based on the COLLADASaxFrameworkLoader|
|dae2ogre        |Demo project that converts COLLADA files to OGRE meshes|
|Externals       |Third party projects required to build the NextGen plug-ins|
|GeneratedSaxParser|Library used to load xml files in the way used by COLLADASaxFrameworkLoader|