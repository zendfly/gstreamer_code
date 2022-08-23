#include <gst/gst.h>

/*
  生成一个用于的测试的视频流

*/

int
main (int argc, char *argv[])
{
  GstElement *pipeline, *source, *sink;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
  // gst_element_factory_make 暂时猜测：根据传入的字符串来创建element（示例），并取一个新的名字用于后续使用
  // 和importlib或者open-mmlib里的注册机制类似
  // 是否可以理解为，各个element提前写好后，在这里通过factory_make进行构建然后时候？
  source = gst_element_factory_make ("videotestsrc", "source");     // videotestsrc：一个生成测试视频的element,用于调试和debug用
  sink = gst_element_factory_make ("autovideosink", "sink");    // autovideosink：在窗口上显示它接收到的图像

  /* Create the empty pipeline */
  pipeline = gst_pipeline_new ("test-pipeline");

  if (!pipeline || !source || !sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  /* Build the pipeline */
  gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);    // 将各个element添加到pipeline中
  if (gst_element_link (source, sink) != TRUE) {      // 使用gst_elemet_link将各个element进行链接(注意顺序)， gst_element_link:first parameter is source, second is destination
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* Modify the source's properties */
  // g_object_set write property, g_object_get get property
  g_object_set (source, "pattern", 0, NULL);     // 将source中的pattern属性设置为0

  /* Start playing */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);    
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);   // start point ？
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);   // 等待执行结束

  /* Parse message */
  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n",
            GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n",
            debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        break;
      default:
        /* We should not reach here because we only asked for ERRORs and EOS */
        g_printerr ("Unexpected message received.\n");
        break;
    }
    gst_message_unref (msg);
  }

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}