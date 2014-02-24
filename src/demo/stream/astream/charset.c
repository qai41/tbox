/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_astream_charset_save_func(tb_size_t state, tb_hize_t size, tb_size_t rate, tb_pointer_t priv)
{
	// trace
	tb_print("save: %llu bytes, rate: %lu bytes/s, state: %s", size, rate, tb_astream_state_cstr(state));

	// kill aicp
	if (state != TB_ASTREAM_STATE_OK) tb_aicp_kill((tb_aicp_t*)priv);

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_astream_charset_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_aicp_t* 		aicp = tb_null;
	tb_handle_t 	tstream = tb_null;
	tb_astream_t* 	istream = tb_null;
	tb_astream_t* 	ostream = tb_null;
	tb_astream_t* 	fstream = tb_null;
	do
	{
		// init aicp
		aicp = tb_aicp_init(2);
		tb_assert_and_check_break(aicp);

		// init istream
		istream = tb_astream_init_from_url(aicp, argv[1]);
		tb_assert_and_check_break(istream);

		// init ostream
		ostream = tb_astream_init_from_file(aicp, argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
		tb_assert_and_check_break(ostream);

		// filter istream or ostream?
		tb_astream_t* iostream = istream;
//		tb_astream_t* iostream = ostream;

		// init fstream
		fstream = tb_astream_init_filter_from_charset(iostream, tb_charset_type(argv[3]), tb_charset_type(argv[4]));
		tb_assert_and_check_break(fstream);

		// init tstream
		if (iostream == istream) tstream = tb_tstream_init_aa(fstream, ostream, -1, tb_demo_astream_charset_save_func, aicp);
		else tstream = tb_tstream_init_aa(istream, fstream, -1, tb_demo_astream_charset_save_func, aicp);
		tb_assert_and_check_break(tstream);

		// limit rate
//		tb_tstream_limit(tstream, 4096);

		// start tstream
		if (!tb_tstream_start(tstream)) break;

		// done loop
		tb_aicp_loop(aicp);

	} while (0);

	// exit tstream
	if (tstream) tb_tstream_exit(tstream, tb_false);
	tstream = tb_null;

	// exit fstream
	if (fstream) tb_astream_exit(fstream, tb_false);
	fstream = tb_null;

	// exit istream
	if (istream) tb_astream_exit(istream, tb_false);
	istream = tb_null;

	// exit ostream
	if (ostream) tb_astream_exit(ostream, tb_false);
	ostream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;
	return 0;
}
