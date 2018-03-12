/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <getopt.h>

#include "cli_description.hh"
#include "thunk/factory.hh"
#include "thunk/ggutils.hh"
#include "thunk/thunk.hh"
#include "util/path.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

void usage( const char * argv0  )
{
  cerr << argv0 << " <cli-description> [program options...]" << endl;
}

void generate_thunk( const CLIDescription & cli_description,
                     const int argc, char * argv[] )
{
  vector<struct option> long_options;
  string optstring;

  for ( const CLIOption & cli_option : cli_description.options() ) {
    if ( cli_option.long_opt.initialized() ) {
      long_options.push_back( { cli_option.long_opt->c_str(), required_argument, nullptr, cli_option.value } );
    }

    if ( cli_option.short_opt.initialized() ) {
      optstring += *cli_option.short_opt;
      optstring += ':';
    }
  }

  long_options.push_back( { nullptr, 0, nullptr, 0 } );

  optind = 1; /* reset getopt */
  opterr = 0; /* turn off error messages */

  vector<string> args = models::args_to_vector( argc, argv,
                                                roost::rbasename( cli_description.target_bin() ).string() );
  vector<ThunkFactory::Data> indata;
  string outfile;

  int opt;
  while ( ( opt = getopt_long( argc, argv, optstring.c_str(), long_options.data(), NULL ) ) != -1 ) {
    for ( const CLIOption & option : cli_description.options() ) {
      if ( option.value == opt ) {
        if ( option.type == CLIOption::Type::OutFile ) {
          outfile = optarg;
        }
        else if ( option.type == CLIOption::Type::InFile ) {
          indata.emplace_back( "", optarg );
          ThunkFactory::Data & this_indata = indata.back();
          args[ ( optind - 1 ) - 1 ] = gg::thunk::GG_HASH_REPLACE + this_indata.hash();
        }
      }
    }
  }

  for ( const size_t idx : cli_description.infile_args() ) {
    indata.emplace_back( "", argv[ optind + idx ] );
    ThunkFactory::Data & this_indata = indata.back();
    args[ ( optind + idx ) - 1 ] = gg::thunk::GG_HASH_REPLACE + this_indata.hash();
  }

  if ( cli_description.outfile_arg().initialized() ) {
    outfile = argv[ optind + *cli_description.outfile_arg() ];
  }

  indata.emplace_back( roost::rbasename( cli_description.target_bin() ).string(),
                        cli_description.target_bin(),
                        ObjectType::Executable );

  ThunkFactory::generate(
    {
      indata.back().hash(),
      args,
      {}
    },
    indata,
    { { "output", outfile } },
    false /* no need to generate a manifest */
  );
}

int main( int argc, char * argv[] )
{
  if ( argc <= 0 ) {
    abort();
  }

  if ( argc < 2 ) {
    usage( argv[ 0 ] );
    return EXIT_FAILURE;
  }

  gg::models::init();

  CLIDescription cli_description { argv[ 1 ] };

  argv++;
  argc--;

  generate_thunk( cli_description, argc, argv );

  return 0;
}
