<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:col="http://www.collada.org/2005/11/COLLADASchema">

  <!-- Attribute List -->
  <xsl:template name="ObjectList">
  
    <!-- Gather attributes into variable -->
    <xsl:variable name="Objects">
      <xsl:for-each select="col:newparam">
        <xsl:call-template name="Object"/>
      </xsl:for-each>
    </xsl:variable>

    <!-- Only display sectionf if data was found -->
    <xsl:if test="normalize-space($Objects)!=''">
      <xsl:value-of select="concat($Tab, 'Objects', '&#xA;')"/>
      <xsl:value-of select="concat($Tab, '{', '&#xA;')"/>
      <xsl:value-of select="$Objects"/>
      <xsl:value-of select="concat($Tab, '}', '&#xA;')"/>
    </xsl:if>

  </xsl:template>
 
  <!-- newparam -->
  <xsl:template name="Object">
    <xsl:variable name="VarType" select="translate(col:annotate[@name='VarType']/col:string, $l, $u)"/>
    <xsl:variable name="ObjectName" select="col:string"/>
    <xsl:variable name="Object" select="col:annotate[@name='Object']/col:string"/>
    <xsl:variable name="ObjectIndex" select="col:annotate[@name='ObjectIndex']/col:string"/>
    <xsl:if test="$VarType='OBJECT' and $Object!='' and $ObjectIndex!='' and $ObjectName!=''">
        <xsl:value-of select="concat($Tabs2, 'Effect_', $Object, ' ', $ObjectIndex, ' ', $ObjectName, '&#xA;')"/>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>