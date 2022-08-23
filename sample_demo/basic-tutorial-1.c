#include <gst/gst.h>

/*
  使用Gstermaer 读取本地/网络视频进行播放

  
*/
int main (int argc, char *argv[])
{
  GstElement *pipeline;   // 数据处理管道
  GstBus *bus;      // 用来管理element的element
  GstMessage *msg; 

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Build the pipeline */
  // pipeline = gst_parse_launch("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);
  pipeline = gst_parse_launch("playbin uri=file:///home/fitow/codeyard/hj/sintel_trailer-480p.webm", NULL);   // run local veido,
  // gst_parse_launch 一个简单、快速、没有太多额外操作的构建pipeline方法，例如上面， pipeline里面只有一个playbin元素，用于播放视频
  // playbin 

  /* Start playing */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);   // 真正使用Gstreamer的时候

  /* See next tutorial for proper error message handling/parsing */
  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
    g_error ("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
        "variable set for more details.");
  }

  /* Free resources */
  gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}
