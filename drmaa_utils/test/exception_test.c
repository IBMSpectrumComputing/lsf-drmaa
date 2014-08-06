
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <drmaa_utils/common.h>


void
raise_ex( int err_code )
{
	fsd_exc_raise_code( err_code );
	printf( "after raise\n" );
	assert(0);
}


void
runner( void (*function)(void) )
{
	int volatile in_except_default_block = 0;
	TRY
	 {
		printf( "runner: before\n" );
		function();
		printf( "runner: after\n" );
		assert(0);
	 }
	EXCEPT_DEFAULT
	 {
		printf( "runner: except: %s\n", fsd_exc_get()->_message );
		in_except_default_block = 1;
	 }
	END_TRY
	
	assert(in_except_default_block);
}


void test_0(void)
{
	TRY
	 { printf( "try\n" ); }
	EXCEPT( FSD_ERRNO_INVALID_VALUE )
	 { 
	    printf( "except\n" ); 
		assert(0);
	 }
	END_TRY
	printf( "after try\n" );
	printf( "test finished.\n" );
}


void test_1(void)
{

	int volatile in_else = 0;
	int volatile in_finally = 0;
	TRY
	 { printf( "try\n" ); }
	EXCEPT_DEFAULT
	 { printf( "except\n" ); assert(0); }
	ELSE
	 { printf( "else\n" ); in_else = 1; }
	FINALLY
	 { printf( "finally\n" ); in_finally = 1; }
	END_TRY
			
	assert(in_else);
	assert(in_finally);
	printf( "test finished.\n" );
}


void test_2(void)
{
	int volatile in_except = 0;
	int volatile in_finally = 0;
	TRY
	 {
		printf( "try\n" );
		raise_ex( FSD_ERRNO_INVALID_VALUE );
		assert(0);
	 }
	EXCEPT_DEFAULT
	 {
		printf( "except: %s\n", fsd_exc_get()->_message );
		in_except = 1;
	 }
	FINALLY
	 {
		printf( "finally\n" );
		in_finally = 1;
	 }
	END_TRY
	
	assert(in_except);
	assert(in_finally);
	printf("test finished\n");
}


void test_3(void)
{
	int volatile  blocks_counter = 0;
	TRY
	 {
		TRY
		 {
			blocks_counter++;
			printf( "inner try\n" );
			raise_ex( FSD_ERRNO_INVALID_VALUE );
			assert(0);
		 }
		FINALLY
		 {
			blocks_counter++;
			printf( "inner finally\n" );
		 }
		END_TRY
		printf( "after\n" );
		assert(0);
	 }
	EXCEPT_DEFAULT
	 {
	    blocks_counter++;
		printf( "except: %s\n", fsd_exc_get()->_message );
		fsd_exc_reraise();
	 }
	ELSE
	 { printf( "else\n" ); assert(0); }
	FINALLY
	 {
		printf( "finally\n" );
		blocks_counter++;
		assert(blocks_counter == 4);
	 }
	END_TRY
	
	assert(0);
}


void test_4(void)
{
	TRY
	 {
		raise_ex( FSD_ERRNO_INVALID_VALUE );
		assert(0);
	 }
	EXCEPT( FSD_ERRNO_INVALID_VALUE )
	 { printf( "except: %s\n", fsd_exc_get()->_message ); }
	EXCEPT_DEFAULT
	 { printf( "except_default: %s\n", fsd_exc_get()->_message ); assert(0); }
	FINALLY
	 {
		printf( "finally\n" );
		raise_ex( FSD_ERRNO_INTERNAL_ERROR );
	 }
	END_TRY
			
	assert(0);
}


int
main( int argc, char *argv[] )
{
	printf("Running Test 0\n");
	test_0();  
	printf("Running Test 1\n");
	test_1();
	printf("Running Test 2\n");
	test_2();  
	printf("Running Test 3\n");
	runner( test_3 );
	printf("Running Test 4\n");
	runner( test_4 );
	printf("All tests done.\n");

	return 0;
}

