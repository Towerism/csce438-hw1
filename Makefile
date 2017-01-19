# List the source file names here for compilation
CRC_FILES = crc.C
CRSD_FILES = crsd.C

# Convenience variable to hold all of the files
FILES = $(CRC_FILES) $(CRSD_FILES)

# Directory names
CRC_DIR = client
CRSD_DIR = server

# Executable names
CRC = crc
CRSD = crsd

# Generate the full path names for our files
CRC_SOURCES = $(patsubst %,$(CRC_DIR)/%,$(CRC_FILES))
CRSD_SOURCES = $(patsubst %,$(CRSD)/%,$(CRSD_FILES))
SOURCES = $(CRC_SOURCES) $(CRSD_SOURCES)

# Tempdir where are object files will be
TEMPDIR = objects

# Generate the object names for our files
CRC_OBJECTS = $(patsubst %.C,$(TEMPDIR)/%.o,$(CRC_FILES))
CRSD_OBJECTS = $(patsubst %.C,$(TEMPDIR)/%.o,$(CRSD_FILES))
OBJECTS = $(CRC_OBJECTS) $(CRSD_OBJECTS)

# Generate the dependencies of our source files
DEPS = $(OBJECTS:.o=.d)

# Conventional environment variables
CXX ?= $(which g++)
CXXFLAGS ?= -std=c++11
LDFLAGS ?=

.DEFAULT: all

all: $(CRC) $(CRSD)

$(TEMPDIR)/%.o: $(CRC_DIR)/%.C
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(TEMPDIR)/%.o: $(CRSD_DIR)/%.C
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Because we can't always guarantee which rule executes first, make directory in both
$(TEMPDIR)/%.d: $(CRC_DIR)/%.C
	mkdir -p $(TEMPDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@
$(TEMPDIR)/%.d: $(CRSD_DIR)/%.C
	mkdir -p $(TEMPDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

$(CRC): $(CRC_OBJECTS)
	$(CXX) $(LDFLAGS) $(CRC_OBJECTS) -o $@

$(CRSD): $(CRSD_OBJECTS)
	$(CXX) $(LDFLAGS) $(CRSD_OBJECTS) -o $@

.PHONY: clean
clean:
	rm -rf $(CRC) $(CRSD) objects

-include $(DEPS)
