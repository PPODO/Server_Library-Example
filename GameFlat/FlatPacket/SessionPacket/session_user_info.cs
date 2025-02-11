// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace FlatPacket.SessionPacket
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct session_user_info : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static session_user_info GetRootAssession_user_info(ByteBuffer _bb) { return GetRootAssession_user_info(_bb, new session_user_info()); }
  public static session_user_info GetRootAssession_user_info(ByteBuffer _bb, session_user_info obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool Verifysession_user_info(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, session_user_infoVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public session_user_info __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Uuid { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public string UserName { get { int o = __p.__offset(6); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetUserNameBytes() { return __p.__vector_as_span<byte>(6, 1); }
#else
  public ArraySegment<byte>? GetUserNameBytes() { return __p.__vector_as_arraysegment(6); }
#endif
  public byte[] GetUserNameArray() { return __p.__vector_as_array<byte>(6); }
  public bool ReadyState { get { int o = __p.__offset(8); return o != 0 ? 0!=__p.bb.Get(o + __p.bb_pos) : (bool)false; } }

  public static Offset<FlatPacket.SessionPacket.session_user_info> Createsession_user_info(FlatBufferBuilder builder,
      int uuid = 0,
      StringOffset user_nameOffset = default(StringOffset),
      bool ready_state = false) {
    builder.StartTable(3);
    session_user_info.AddUserName(builder, user_nameOffset);
    session_user_info.AddUuid(builder, uuid);
    session_user_info.AddReadyState(builder, ready_state);
    return session_user_info.Endsession_user_info(builder);
  }

  public static void Startsession_user_info(FlatBufferBuilder builder) { builder.StartTable(3); }
  public static void AddUuid(FlatBufferBuilder builder, int uuid) { builder.AddInt(0, uuid, 0); }
  public static void AddUserName(FlatBufferBuilder builder, StringOffset userNameOffset) { builder.AddOffset(1, userNameOffset.Value, 0); }
  public static void AddReadyState(FlatBufferBuilder builder, bool readyState) { builder.AddBool(2, readyState, false); }
  public static Offset<FlatPacket.SessionPacket.session_user_info> Endsession_user_info(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<FlatPacket.SessionPacket.session_user_info>(o);
  }
  public static void Finishsession_user_infoBuffer(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.session_user_info> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedsession_user_infoBuffer(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.session_user_info> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class session_user_infoVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Uuid*/, 4 /*int*/, 4, false)
      && verifier.VerifyString(tablePos, 6 /*UserName*/, false)
      && verifier.VerifyField(tablePos, 8 /*ReadyState*/, 1 /*bool*/, 1, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
