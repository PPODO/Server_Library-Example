// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace FlatPacket.SessionPacket
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct CreateSession : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static CreateSession GetRootAsCreateSession(ByteBuffer _bb) { return GetRootAsCreateSession(_bb, new CreateSession()); }
  public static CreateSession GetRootAsCreateSession(ByteBuffer _bb, CreateSession obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool VerifyCreateSession(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, CreateSessionVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public CreateSession __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int RequestedUserUuid { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public string SessionName { get { int o = __p.__offset(6); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetSessionNameBytes() { return __p.__vector_as_span<byte>(6, 1); }
#else
  public ArraySegment<byte>? GetSessionNameBytes() { return __p.__vector_as_arraysegment(6); }
#endif
  public byte[] GetSessionNameArray() { return __p.__vector_as_array<byte>(6); }

  public static Offset<FlatPacket.SessionPacket.CreateSession> CreateCreateSession(FlatBufferBuilder builder,
      int requested_user_uuid = 0,
      StringOffset session_nameOffset = default(StringOffset)) {
    builder.StartTable(2);
    CreateSession.AddSessionName(builder, session_nameOffset);
    CreateSession.AddRequestedUserUuid(builder, requested_user_uuid);
    return CreateSession.EndCreateSession(builder);
  }

  public static void StartCreateSession(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddRequestedUserUuid(FlatBufferBuilder builder, int requestedUserUuid) { builder.AddInt(0, requestedUserUuid, 0); }
  public static void AddSessionName(FlatBufferBuilder builder, StringOffset sessionNameOffset) { builder.AddOffset(1, sessionNameOffset.Value, 0); }
  public static Offset<FlatPacket.SessionPacket.CreateSession> EndCreateSession(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<FlatPacket.SessionPacket.CreateSession>(o);
  }
  public static void FinishCreateSessionBuffer(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.CreateSession> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedCreateSessionBuffer(FlatBufferBuilder builder, Offset<FlatPacket.SessionPacket.CreateSession> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class CreateSessionVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*RequestedUserUuid*/, 4 /*int*/, 4, false)
      && verifier.VerifyString(tablePos, 6 /*SessionName*/, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
