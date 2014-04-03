<?php
 
error_reporting( E_NONE );
 
define( 'JAIL', '/path/to/jail/where/files/to/be/served/are/in' );
 
if( !isset( $_GET['f'] ) || empty( $_GET['f'] ) )
{
	die( 'wtf ?' );
}
	
$path = JAIL.DIRECTORY_SEPARATOR.$_GET['f'];
$path = realpath( $path );

if( !$path || !strstr( $path, JAIL ) )
{
	die( 'security breach ? you dare to ?' );
}

if( !isset( $_GET['a'] ) )
{
	die( 'what to do ? what to do ?' );
}

switch( $_GET['a'] )
{
	case 'v':
	
	$r = include( 'geshi/geshi.php' );
	if( $r !== 1 )
	{
		die( 'oops! hl is broken!');
	}
	$t = false;
	if( isset( $_GET['t'] ) && !empty( $_GET['t'] ) )
		$t = $_GET['t'];

	$geshi = new geshi( file_get_contents( $path ), $t );
	
	echo '<html><head><title>'.basename( $path ).'</title></head><body>';
	echo $geshi->parse_code();
	echo '</body></html>';

	break;
	
	case 'd':
	
		if( !is_readable( $path ) )
			die( 'i cannot read!' );
			
		header( 'Pragma: public' );
		header( 'Expires: 0' );
		header( 'Cache-Control: must-revalidate, post-check=0, pre-check=0' );

		header( 'Content-Type: application/force-download' );
		header( 'Content-Type: application/octet-stream' );
		header( 'Content-Type: application/download' );

		header( 'Content-Disposition: attachment; filename='.basename($path).';' );

		header( 'Content-Transfer-Encoding: binary' );
		header( 'Content-Length: '.filesize($path) );

		if( !readfile($path) )
		{
			echo 'ERROR: Could not read input. This downloaded file is BROKEN.\n';
		}
		
	break;
	default:
		die( 'you want what?' );
}

 
?>