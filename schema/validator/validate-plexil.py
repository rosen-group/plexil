#! /usr/bin/env python3

import click
import os.path as path
import sys
import xml.etree.ElementTree as ElementTree
import xmlschema

defaultSchemaFile = path.join(path.dirname(path.dirname(path.realpath(__file__))),
                              'core-plexil.xsd')

@click.command()

@click.option('-s', '--silent',
              is_flag=True,
              help="Don't generate output, only set exit status")

@click.option('-v', '--verbose',
              is_flag=True,
              help="Generate extra output")

@click.option('--schema',
              type=click.Path(exists=True),
              default=defaultSchemaFile,
              help="Schema file to validate against")

@click.argument('infiles',
                type=click.Path(exists=True),
                nargs=-1)

# Main function
def validate(infiles, schema, silent, verbose):
    """Validate XML files against a schema."""

    schm = loadSchema(schema)
    if schm is None:
        sys.exit(1)

    if verbose:
        print('Validating against schema', schema)

    nFiles = len(infiles)
    nValid = 0
    nInvalid = 0
    nError = 0

    for infile in infiles:
        if silent:
            status = validateSilently(schm, infile)
        else:
            status = validateWithErrorMessages(schm, infile, verbose)
        if status > 0:
            nInvalid = nInvalid + 1
            if not silent:
                print(infile, 'failed validation with', status, 'errors')
        elif status < 0:
            nError = nError + 1
            if not silent:
                print(infile, 'could not be validated due to error')
        else:
            nValid = nValid + 1
            if not silent:
                print(infile, 'is valid')

    if nFiles > 1 and not silent:
        print(nFiles, 'files checked,', nValid, 'valid')
        if nInvalid > 0:
            print('  ', nInvalid, 'files invalid')
        if nError > 0:
            print('  ', nError, 'could not be validated')

    if nError > 0:
        sys.exit(2)
    elif nInvalid > 0:
        sys.exit(1)
    else:
        sys.exit(0)
        

# Load the named schema file and return it.
# Return None in event of error.
def loadSchema(schema):
    try:
        return xmlschema.XMLSchema11(schema, validation='lax')

    except xmlschema.exceptions.XMLSchemaException as x:
        print('Error reading schema ', schema, ':\n ', str(x),
              sep='', file=sys.stderr)
        return None;

    except ElementTree.ParseError as p:
        print('XML parse error reading schema ', schema, ':\n ', str(p),
              sep='', file=sys.stderr)
        return None;

    except Exception as e:
        print('Error of type ', type(e).__name__, ' reading schema ', schema, ':\n ', str(e),
              sep='', file=sys.stderr)
        return None;

# Validate the file, returning 0 if valid,
# 1 if invalid, or -1 if an exception occurred
def validateSilently(schm, infile):
    try:
        if schm.is_valid(infile):
            return 0
        else:
            return 1

    except ElementTree.ParseError as p:
        print(infile, 'could not be parsed as XML')
        return -1

    except Exception as e:
        print('Unknown error while validating ', infile, ':\n ', str(e),
              sep='', file=sys.stderr)
        return -1

# Validate the file, printing each validation error
# Return the number of errors, or -1 if some other exception occurred
def validateWithErrorMessages(schm, infile, verbose):
    if verbose:
        print('Validating', infile)

    errorCount = 0

    try:
        for err in schm.iter_errors(infile):
            errorCount = errorCount + 1
            if verbose:
                print('  ', str(err))
            elif err.path is not None:
                print('  ', err.message, '\n  at', err.path)
            else:
                print('  ', err.message)
        return errorCount

    except ElementTree.ParseError as p:
        print('XML parse error for ', infile, ':\n ', str(p),
              sep='', file=sys.stderr)
        return -1

    except Exception as e:
        print('Unknown error while validating ', infile, ':\n ', str(e),
              sep='', file=sys.stderr)
        return -1
        
if __name__ == '__main__':
    validate()
